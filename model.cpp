/** 
 *  @file           model.cpp
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          Board model implementation file
 */
#include "model.h"
#include "filter.h"
#include "can_servizi.h"

#include <DueFlashStorage.h>

#undef HID_ENABLED

/** @addtogroup Board_model_group
 *  
 *  @{
 */

/**
 *  @def ADC_BUFFER_SIZE
 *  @brief Size (bytes) of buffer for store each ADC channel data
 */
#define ADC_BUFFER_SIZE         128

/**
 *  @def BUFFERs
 *  @brief Number of ADC buffers
 */
#define BUFFERS                 4

/**
 *  @def ADC_MIN
 *  @brief ADC lower bound value
 */
#define ADC_MIN                 0

/**
 *  @def ADC_MAX
 *  @brief ADC upper bound value
 */
#define ADC_MAX                 4095

/**
 *  @def APPS_PLAUS_RANGE
 *  @brief Size (bytes) of each ADC buffer
 */
#define APPS_PLAUS_RANGE        10

/**
 *  @def ADC_CHANNELS_LIST
 *  @brief List of ADC channels dedicated to each board pinout
 */
#define ADC_CHANNELS_LIST       TPS1_ADC_CHAN_NUM | TPS2_ADC_CHAN_NUM | BRAKE_ADC_CHAN_NUM | SC_ADC_CHAN_NUM

/**
 *  @def ADC_CHANNELS
 *  @brief Number of ADC channels
 */
#define ADC_CHANNELS            4

/**
 *  @def TPS1_ADC_OFFSET
 *  @brief Offset from DMA buffer
 */
#define TPS1_ADC_OFFSET         0

/**
 *  @def TPS2_ADC_OFFSET
 *  @brief Offset from DMA buffer
 */
#define TPS2_ADC_OFFSET         1

/**
 *  @def BRAKE_ADC_OFFSET
 *  @brief Offset from DMA buffer
 */
#define BRAKE_ADC_OFFSET        2

/**
 *  @def SC_ADC_OFFSET
 *  @brief Offset from DMA buffer
 */
#define SC_ADC_OFFSET           3

/**
 *  @def BUFFER_LENGTH
 *  @brief Length, in bytes, of each DMA buffer
 */
#define BUFFER_LENGTH           ADC_BUFFER_SIZE * ADC_CHANNELS

/**
 * @var     volatile uint8_t tps1_percentage;
 * @brief   First APPS percentage value retrieved by analog tps1 signal (#TPS1_PIN)
 */
volatile uint8_t    tps1_adc_percentage    = 0;

/**
 * @var     volatile uint8_t tps2_percentage;
 * @brief   Second APPS percentage value retrieved by analog tps2 signal (#TPS2_PIN)
 */
volatile uint8_t    tps2_adc_percentage    = 0;

/**
 * @var     volatile uint8_t brake_percentage;
 * @brief   Brake pedal position sensor percentage value retrieved by analog brake
 *          signal (#BRAKE_PIN)
 */
volatile uint8_t    brake_adc_percentage   = 0;

/**
 * @var     volatile bool apps_adc_plausibility;
 * @brief   APPS plausibility status retrieved by analog acquisition
 */
volatile bool       apps_adc_plausibility  = true;

/**
 * @var     volatile bool brake_adc_plausibility;
 * @brief   Brake plausibility status retrieved by analog acquisition
 */
volatile bool       brake_adc_plausibility = true;

/**
 * @var     volatile uint16_t tps1_value;
 * @brief   First APPS value retrieved directly by analog tps1 signal (#TPS1_PIN)
 *          and filtered after DMA buffer is filled entirely
 */
volatile uint16_t   tps1_value = 0;

/**
 * @var     volatile uint16_t tps2_value;
 * @brief   Second APPS value retrieved directly by analog tps2 signal (#TPS2_PIN)
 *          and filtered after DMA buffer is filled entirely
 */
volatile uint16_t   tps2_value = 0;

/**
 * @var     volatile uint16_t brake_value;
 * @brief   Brake pedal position sensor value retrieved directly by analog brake
 *          signal (#BRAKE_PIN) and filtered after DMA buffer is filled entirely
 */
volatile uint16_t   brake_value = 0;

/**
 * @var     volatile uint16_t SC_value;
 * @brief   SC value retrieved directly by analog SC signal (#SC_PIN)
 *          and filtered after DMA buffer is filled entirely
 */
volatile uint16_t   SC_value    = 0;

/**
 * @def     TPS1_UPPER_BOUND
 * @brief   First APPS max output voltage (3V)
 */
#define TPS1_UPPER_BOUND            3723

/**
 * @def     TPS1_LOWER_BOUND
 * @brief   First APPS min output voltage (0.8V)
 */
#define TPS1_LOWER_BOUND            993

/**
 * @def     TPS2_UPPER_BOUND
 * @brief   Second APPS max output voltage (1.5V)
 */
#define TPS2_UPPER_BOUND            1861

/**
 * @def     TPS2_LOWER_BOUND
 * @brief   Second APPS min output voltage (0.4V)
 */
#define TPS2_LOWER_BOUND            496

/**
 * @def     BRAKE_UPPER_BOUND
 * @brief   Brake sensor max output voltage (TODO: check Voutmax)
 */
#define BRAKE_UPPER_BOUND           0

/**
 * @def     BRAKE_LOWER_BOUND
 * @brief   Brake sensor min output voltage (TODO: check Voutmin)
 */
#define BRAKE_LOWER_BOUND           ADC_MAX

#if 0
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
#endif

// pedals ranges in RAM
volatile uint16_t   tps1_max    = TPS1_UPPER_BOUND;
volatile uint16_t   tps1_low    = TPS1_LOWER_BOUND;
volatile uint16_t   tps2_max    = TPS2_UPPER_BOUND;
volatile uint16_t   tps2_low    = TPS2_LOWER_BOUND;
volatile uint16_t   brake_max   = BRAKE_UPPER_BOUND;
volatile uint16_t   brake_low   = BRAKE_LOWER_BOUND;

volatile int        bufn, obufn;

/**
 * @var     volatile uint16_t   buf[#BUFFERS][#BUFFER_LENGTH];
 * @brief   DMA buffers: #BUFFERS number of buffers each of #BUFFER_LENGTH size;
 *          DMA is configured in cyclic mode: after one of #BUFFERS is filled then 
 *          DMA transfer head moves to next buffer in circular indexing.
 */
volatile uint16_t   buf[BUFFERS][BUFFER_LENGTH];

volatile bool       calibrate          = false;

#if 0
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
#endif

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

#if 0
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
#endif

/**
 *  @}
 */