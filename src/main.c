#include "n32g031.h"
#include "n32g031_gpio.h"
#include "n32g031_rcc.h"
#include "n32g031_adc.h" 


/* ====================================================================
 * ⚙️ [โซนตั้งค่าฮาร์ดแวร์] 
 * ==================================================================== */
#define PULSE_MIN     2250
#define PULSE_MAX     5500
#define KNOB_MAX      4095

/* ====================================================================
 * [โซนหลังบ้าน] ฟังก์ชันผู้ช่วยของพี่ AI
 * ==================================================================== */
void Delay_Loop(volatile uint32_t count) {
    while(count--) { __NOP(); }
}

/* ฟังก์ชัน Kalman Filter สำหรับกรองสัญญาณรบกวน (1D Simple Kalman) */
float kalman_update(float measurement, float* state, float* pc) {
    float k_gain = *pc / (*pc + 0.1f);     // 0.1 คือ R (Measurement Noise) ปรับเพิ่มถ้าวอลลุ่มแกว่งมาก
    *pc = (1.0f - k_gain) * (*pc) + 0.01f; // 0.01 คือ Q (Process Noise)
    *state = *state + k_gain * (measurement - *state);
    return *state;
}

/* ฟังก์ชันตั้งค่าตัวหมุน (ADC) */
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

/* ฟังก์ชันอ่านค่าจากตัวหมุน (คืนค่าเป็นตัวเลข 0 ถึง 4095) */
uint32_t Read_Knob(void) {
    ADC_ConfigRegularChannel(ADC, ADC_CH_0, 1, ADC_SAMP_TIME_56CYCLES5);
    ADC_EnableSoftwareStartConv(ADC, ENABLE);
    while(ADC_GetFlagStatus(ADC, ADC_FLAG_ENDC) == RESET);
    ADC_ClearFlag(ADC, ADC_FLAG_ENDC);
    return ADC_GetDat(ADC);
}

/* ฟังก์ชันส่งสัญญาณให้เซอร์โว */
void Servo_Step(uint32_t pulse_width) {
    GPIO_SetBits(GPIOA, GPIO_PIN_1);     
    Delay_Loop(pulse_width);             
    GPIO_ResetBits(GPIOA, GPIO_PIN_1);   
    Delay_Loop(100000);                  
}

/* ====================================================================
 * 🚀 [โซนทำงานหลัก]
 * ==================================================================== */
int main(void) {
    /* 1. เปิดสวิตช์เตรียมความพร้อมให้อุปกรณ์ */
    Setup_Knob(); // เตรียมตัวหมุน (PA0)
    
    // เตรียมแขนเซอร์โว (PA1)
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    uint32_t raw_knob = 0;
    uint32_t target_pulse = PULSE_MIN;
    
    // ตัวแปรสถานะสำหรับ Kalman Filter
    float k_state = (float)Read_Knob(); // อ่านค่าเริ่มต้นเพื่อไม่ให้เซอร์โวกระตุก
    float k_pc = 0.0f;                  // ค่าความคลาดเคลื่อนเริ่มต้น

    /* 2. ลูปการทำงานหลัก (บิดวอลลุ่ม ควบคุมเซอร์โว!) */
    while(1) {
        // ก. อ่านค่าดิบจากมือเราที่บิดตัวหมุน
        raw_knob = Read_Knob(); 
        
        // ข. กรองสัญญาณด้วย Kalman Filter
        kalman_update((float)raw_knob, &k_state, &k_pc);
        
        // ค. แปลงค่าตัวหมุนที่กรองแล้ว ให้กลายเป็นองศาเซอร์โว (กลับทิศทาง)
        target_pulse = PULSE_MAX - (((uint32_t)k_state * (PULSE_MAX - PULSE_MIN)) / KNOB_MAX);
        
        // ง. ส่งคำสั่งไปบอกเซอร์โวให้ขยับไปที่ตำแหน่งนั้น
        Servo_Step(target_pulse);
    }
}