// UnityMbed — Real-time Servo Control with Potentiometer (ADC) @ N32G031
#include "n32g031.h"
#include "n32g031_gpio.h"
#include "n32g031_rcc.h"
#include "n32g031_adc.h" 

/* ====================================================================
 * [Hardware Configuration Zone]
 * ==================================================================== */
#define PULSE_MIN     2250
#define PULSE_MAX     5500
#define KNOB_MAX      4095

/* Simple software delay loop */
void Delay_Loop(volatile uint32_t count) {
    while(count--) { __NOP(); }
}

/* Kalman Filter function for sensor noise rejection (1D Simple Kalman) */
float kalman_update(float measurement, float* state, float* pc) {
    float k_gain = *pc / (*pc + 0.1f);     // 0.1 is R (Measurement Noise)
    *pc = (1.0f - k_gain) * (*pc) + 0.01f; // 0.01 is Q (Process Noise)
    *state = *state + k_gain * (measurement - *state);
    return *state;
}

/* Initialize Potentiometer ADC on PA0 (ADC Channel 0) */
void Setup_Knob(void) {
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_ADC, ENABLE);
    
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    ADC_InitType ADC_InitStructure;
    ADC_InitStruct(&ADC_InitStructure);
    ADC_InitStructure.MultiChEn      = DISABLE;
    ADC_InitStructure.ContinueConvEn = DISABLE;
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIGCONV_NONE;
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber      = 1;
    ADC_Init(ADC, &ADC_InitStructure);
    ADC_Enable(ADC, ENABLE);
}

/* Read analog value from potentiometer knob (Returns 0 to 4095) */
uint32_t Read_Knob(void) {
    ADC_ConfigRegularChannel(ADC, ADC_CH_0, 1, ADC_SAMP_TIME_56CYCLES5);
    ADC_EnableSoftwareStartConv(ADC, ENABLE);
    while(ADC_GetFlagStatus(ADC, ADC_FLAG_ENDC) == RESET);
    ADC_ClearFlag(ADC, ADC_FLAG_ENDC);
    return ADC_GetDat(ADC);
}

/* Transmit control signal pulse to servo motor */
void Servo_Step(uint32_t pulse_width) {
    GPIO_SetBits(GPIOA, GPIO_PIN_1);     
    Delay_Loop(pulse_width);             
    GPIO_ResetBits(GPIOA, GPIO_PIN_1);   
    Delay_Loop(100000);                  
}

/* ====================================================================
 * Main Application Loop
 * ==================================================================== */
int main(void) {
    /* 1. Initialize peripherals */
    Setup_Knob(); // Initialize Potentiometer ADC (PA0)
    
    // Initialize Servo Motor Control Pin (PA1)
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    uint32_t raw_knob = 0;
    uint32_t target_pulse = PULSE_MIN;
    
    // Kalman Filter state variables
    float k_state = (float)Read_Knob(); // Read initial baseline to prevent servo jitter
    float k_pc = 0.0f;                  // Initial error covariance

    /* 2. Main Tracking Loop: Turn knob to position servo in real time */
    while(1) {
        // Read raw ADC sample from potentiometer knob
        raw_knob = Read_Knob(); 
        
        // Filter out analog noise with Kalman Filter
        kalman_update((float)raw_knob, &k_state, &k_pc);
        
        // Map filtered knob reading to calibrated servo pulse width (inverted)
        target_pulse = PULSE_MAX - (((uint32_t)k_state * (PULSE_MAX - PULSE_MIN)) / KNOB_MAX);
        
        // Output pulse to update servo position
        Servo_Step(target_pulse);
    }
}
