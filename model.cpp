#include "model.h"
#include "filter.h"
#include "can_servizi.h"

#include <DueFlashStorage.h>

#undef HID_ENABLED
#define ADC_BUFFER_SIZE         128
#define BUFFERS                 4

#define ADC_MIN                 0
#define ADC_MAX                 4095 // TODO: abbassare a 3V

#define APPS_MIN                (ADC_MAX >> 1)
#define APPS_MAX                (ADC_MAX >> 1)

#define APPS_PLAUS_RANGE        10

#define ADC_CHANNELS_LIST       TPS1_ADC_CHAN_NUM | TPS2_ADC_CHAN_NUM | BRAKE_ADC_CHAN_NUM | SC_ADC_CHAN_NUM
#define ADC_CHANNELS            4

#define TPS1_ADC_OFFSET         0
#define TPS2_ADC_OFFSET         1
#define BRAKE_ADC_OFFSET        2
#define SC_ADC_OFFSET           3

#define BUFFER_LENGTH           ADC_BUFFER_SIZE * ADC_CHANNELS

// analog acquisition
volatile uint8_t    tps1_adc_percentage    = 0;
volatile uint8_t    tps2_adc_percentage    = 0;
volatile uint8_t    brake_adc_percentage   = 0;
volatile bool       apps_adc_plausibility  = false;
volatile bool       brake_adc_plausibility = false;

volatile uint16_t   tps1_value = 0;
volatile uint16_t   tps2_value = 0;
volatile uint16_t   brake_value = 0;
volatile uint16_t   SC_value    = 0;

// TODO: vedere soglie fisse dei pedali
#define TPS1_UPPER_BOUND            APPS_MIN
#define TPS1_LOWER_BOUND            APPS_MAX

#define TPS2_UPPER_BOUND            APPS_MIN
#define TPS2_LOWER_BOUND            APPS_MAX

#define BRAKE_UPPER_BOUND           APPS_MIN
#define BRAKE_LOWER_BOUND           APPS_MAX

// struct for loading/storing pedals ranges in flash memory
typedef struct pedals_ranges_s {
    volatile uint16_t   tps1_max;
    volatile uint16_t   tps1_low;
    volatile uint16_t   tps2_max;
    volatile uint16_t   tps2_low;
    volatile uint16_t   brake_max;
    volatile uint16_t   brake_low;
} pedals_ranges_t;

// pedals ranges' addresses in flash memory
#define FIRST_RUN_FLAG_ADDR         0
#define PEDALS_RANGES_FLASH_ADDR    4

DueFlashStorage     dueFlashStorage;

// pedals ranges in RAM
volatile uint16_t   tps1_max    = TPS1_UPPER_BOUND;
volatile uint16_t   tps1_low    = TPS1_LOWER_BOUND;
volatile uint16_t   tps2_max    = TPS2_UPPER_BOUND;
volatile uint16_t   tps2_low    = TPS2_LOWER_BOUND;
volatile uint16_t   brake_max   = BRAKE_UPPER_BOUND;
volatile uint16_t   brake_low   = BRAKE_LOWER_BOUND;

volatile int        bufn, obufn;

volatile uint16_t   buf[BUFFERS][BUFFER_LENGTH];

volatile bool       calibrate          = false;

static inline void load_pedals_ranges(pedals_ranges_t* ranges) {
    uint8_t first_time = dueFlashStorage.read(FIRST_RUN_FLAG_ADDR);
    if (first_time) {
        ranges -> tps1_low = TPS1_LOWER_BOUND;
        ranges -> tps1_max = TPS1_UPPER_BOUND;
        ranges -> tps2_low = TPS2_LOWER_BOUND;
        ranges -> tps2_max = TPS2_UPPER_BOUND;
        ranges -> brake_low = BRAKE_LOWER_BOUND;
        ranges -> brake_max = BRAKE_UPPER_BOUND;

        byte buf[sizeof(pedals_ranges_t)];
        memcpy(buf, ranges, sizeof(pedals_ranges_t));
        dueFlashStorage.write(PEDALS_RANGES_FLASH_ADDR, buf, sizeof(pedals_ranges_t));
        dueFlashStorage.write(FIRST_RUN_FLAG_ADDR, 0);
    } else {
        byte* buf = dueFlashStorage.readAddress(PEDALS_RANGES_FLASH_ADDR);
        memcpy(ranges, buf, sizeof(pedals_ranges_t));
    }
}

static inline void store_pedals_ranges(pedals_ranges_t* ranges) {
    byte buf[sizeof(pedals_ranges_t)];
    memcpy(buf, ranges, sizeof(pedals_ranges_t));
    dueFlashStorage.write(PEDALS_RANGES_FLASH_ADDR, buf, sizeof(pedals_ranges_t)); // write byte array to flash
}

static inline void filter_data() {
    tps1_value = (tps1_value + filter_buffer(buf[obufn] + TPS1_ADC_OFFSET, ADC_BUFFER_SIZE, ADC_CHANNELS)) / 2;
    tps2_value = (tps2_value + filter_buffer(buf[obufn] + TPS2_ADC_OFFSET, ADC_BUFFER_SIZE, ADC_CHANNELS)) / 2;
    brake_value = (brake_value + filter_buffer(buf[obufn] + BRAKE_ADC_OFFSET, ADC_BUFFER_SIZE, ADC_CHANNELS)) / 2;
    SC_value = (SC_value + filter_buffer(buf[obufn] + SC_ADC_OFFSET, ADC_BUFFER_SIZE, ADC_CHANNELS)) / 2;

    if (calibrate) {
        if (tps1_value < tps1_low) tps1_low = tps1_value;
        if (tps1_value > tps1_max) tps1_max = tps1_value;
        if (tps2_value < tps2_low) tps2_low = tps2_value;
        if (tps2_value > tps2_max) tps2_max = tps2_value;
        if (brake_value < brake_low) brake_low = brake_value;
        if (brake_value > brake_max) brake_max = brake_value;
    }

    tps1_adc_percentage = map(tps1_value, tps1_low, tps1_max, 0, 100);
    tps2_adc_percentage = map(tps2_value, tps2_low, tps2_max, 0, 100);
    brake_adc_percentage = map(brake_value, brake_low, brake_max, 0, 100);

    // check APPS plausibility
    if (abs(tps1_adc_percentage - tps2_adc_percentage) > APPS_PLAUS_RANGE)
      apps_adc_plausibility = false;
    else
      apps_adc_plausibility = true;

    // check APPS + brake plausibility (BSPD)
    if (tps1_adc_percentage > 5 && brake_adc_percentage > 25) // ACCELERATOR + BRAKE plausibility
      brake_adc_plausibility = false;
    else if (!brake_adc_percentage)
      brake_adc_plausibility = true;
}

void ADC_Handler() {
    int f = ADC->ADC_ISR;
    if (f & (1 << 27)){
        bufn = (bufn + 1) &(BUFFERS - 1); // move DMA pointers to next buffer
        ADC->ADC_RNPR = (uint32_t) buf[bufn];
        ADC->ADC_RNCR = BUFFER_LENGTH;

        filter_data();
    
        obufn = (obufn + 1) &(BUFFERS - 1);
    }
}

void model_init() {
    pmc_enable_periph_clk(ID_ADC);
    adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP_FAST);
    adc_set_resolution(ADC, ADC_12_BITS);
    ADC->ADC_MR |=0x80; // free running

    ADC->ADC_CHER = ADC_CHANNELS_LIST;

    NVIC_EnableIRQ(ADC_IRQn);
    ADC->ADC_IDR = ~ADC_IDR_ENDRX; //~(1<<27); End of Receive Buffer Interrupt Disable
    ADC->ADC_IER = ADC_IDR_ENDRX; // 1<<27; End of Receive Buffer Interrupt Enable
    ADC->ADC_RPR = (uint32_t) buf[0];   // DMA buffer
    ADC->ADC_RCR = BUFFER_LENGTH;
    ADC->ADC_RNPR = (uint32_t) buf[1]; // next DMA buffer
    ADC->ADC_RNCR = BUFFER_LENGTH;
    bufn = obufn = 1;
    ADC->ADC_PTCR = 1;
    ADC->ADC_CR = 2;

    pinMode(FAN, OUTPUT);
    pinMode(AIRcc, OUTPUT);
    pinMode(AIRGnd, OUTPUT);
    pinMode(PRE, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    pinMode(AIRB, INPUT_PULLUP);
    pinMode(RTDB, INPUT_PULLUP);

    Serial.begin(SERIAL_BAUDRATE);

    analogWriteResolution(12);

    // model_enable_calibrations();
}

__attribute__((__inline__)) volatile uint8_t get_tps1_percentage() {
    return can_servizi_online() ? get_servizi_tps1() : tps1_adc_percentage;
}

__attribute__((__inline__)) volatile uint8_t get_tps2_percentage() {
    return can_servizi_online() ? get_servizi_tps2() : tps2_adc_percentage;
}

__attribute__((__inline__)) volatile uint8_t get_brake_percentage() {
    return can_servizi_online() ? get_servizi_brake() : brake_adc_percentage;
}

__attribute__((__inline__)) volatile bool    get_apps_plausibility() {
    return can_servizi_online() ? get_servizi_apps_plausibility() : apps_adc_plausibility;
}

__attribute__((__inline__)) volatile bool    get_brake_plausibility() {
    return can_servizi_online() ? get_servizi_brake_plausibility() : brake_adc_plausibility;
}

__attribute__((__inline__)) volatile uint16_t get_SC_value() {
    return SC_value;
}

// load previous calibration from flash & enable calibration
__attribute__((__inline__))
void model_enable_calibrations() {
    pedals_ranges_t     pedals_ranges;

    load_pedals_ranges(&pedals_ranges);

    tps1_low = pedals_ranges.tps1_low;
    tps1_max = pedals_ranges.tps1_max;
    tps2_low = pedals_ranges.tps2_low;
    tps2_max = pedals_ranges.tps2_max;
    brake_low = pedals_ranges.brake_low;
    brake_max = pedals_ranges.brake_max;

    calibrate = true;
}

__attribute__((__inline__))
void model_disable_calibrations() {
    pedals_ranges_t     pedals_ranges;

    calibrate = false;

    pedals_ranges.tps1_low = tps1_low;
    pedals_ranges.tps1_max = tps1_max;
    pedals_ranges.tps2_low = tps2_low;
    pedals_ranges.tps2_max = tps2_max;
    pedals_ranges.brake_low = brake_low;
    pedals_ranges.brake_max = brake_max;

    store_pedals_ranges(&pedals_ranges);
}