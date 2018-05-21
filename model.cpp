#include "model.h"

#undef HID_ENABLED
#define ADC_BUFFER_SIZE         128
#define BUFFERS                 4

#define ADC_MIN                 0
#define ADC_MAX                 4095

#define APPS_PLAUS_RANGE        10

#define ADC_CHANNELS            3
#define ADC_CHANNELS_LIST       TPS1_ADC_CHAN_NUM | TPS2_ADC_CHAN_NUM | BRAKE_ADC_CHAN_NUM
  
#define BUFFER_LENGTH           ADC_BUFFER_SIZE * ADC_CHANNELS

// CAN acquisition
volatile uint8_t    tps1_percentage    = 0;
volatile uint8_t    tps2_percentage    = 0;
volatile uint8_t    brake_percentage   = 0;
volatile bool       apps_plausibility  = true;
volatile bool       brake_plausibility = true;

// analog acquisition
volatile uint8_t    tps1_adc_percentage    = 0;
volatile uint8_t    tps2_adc_percentage    = 0;
volatile uint8_t    brake_adc_percentage   = 0;
volatile bool       apps_adc_plausibility  = true;
volatile bool       brake_adc_plausibility = true;

volatile uint16_t   tps1_value = 0;
volatile uint16_t   tps2_value = 0;
volatile uint16_t   brake_value = 0;

volatile uint16_t   tps1_max = ADC_MIN;
volatile uint16_t   tps1_low = ADC_MAX;

volatile uint16_t   tps2_max = ADC_MIN;
volatile uint16_t   tps2_low = ADC_MAX;

volatile uint16_t   brake_max = ADC_MIN;
volatile uint16_t   brake_low = ADC_MAX;

volatile int        bufn, obufn;

volatile uint16_t   buf[BUFFERS][BUFFER_LENGTH];

volatile bool       calibrate          = true;


static inline void filter_data() {
    tps1_value = (tps1_value + filter_buffer(buf[obufn] + TPS1_ADC_OFFSET, ADC_BUFFER_SIZE, ADC_CHANNELS)) / 2;
    tps2_value = (tps2_value + filter_buffer(buf[obufn] + TPS2_ADC_OFFSET, ADC_BUFFER_SIZE, ADC_CHANNELS)) / 2;
    brake_value = (brake_value + filter_buffer(buf[obufn] + BRAKE_ADC_OFFSET, ADC_BUFFER_SIZE, ADC_CHANNELS)) / 2;

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
    else if (!brake_percentage)
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

    // for regen brake
    analogWriteResolution(12);
}

__attribute__((__inline__)) volatile uint8_t get_tps1_percentage() {
    return tps1_adc_percentage;
}

__attribute__((__inline__)) volatile uint8_t get_tps2_percentage() {
    return tps2_adc_percentage;
}

__attribute__((__inline__)) volatile uint8_t get_brake_percentage() {
    return brake_adc_percentage;
}

__attribute__((__inline__)) volatile bool    get_apps_plausibility() {
    return apps_adc_plausibility;
}

__attribute__((__inline__)) volatile bool    get_brake_plausibility() {
    return brake_adc_plausibility;
}