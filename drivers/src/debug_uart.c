#include "debug_uart.h"
#include "n32g031.h"
#include "n32g031_rcc.h"
#include "n32g031_gpio.h"
#include "n32g031_usart.h"

void Debug_UART_Init(void) {
    GPIO_InitType GPIO_InitStructure;
    USART_InitType USART_InitStructure;

    // Enable clocks for GPIOA and USART1
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_USART1, ENABLE);

    GPIO_InitStruct(&GPIO_InitStructure);
    
    // Configure PA9 as USART1_TX
    GPIO_InitStructure.Pin            = GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF4_USART1;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    // Configure PA10 as USART1_RX
    GPIO_InitStructure.Pin            = GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF4_USART1;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    // Configure USART1
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.BaudRate            = 115200;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

    USART_Init(USART1, &USART_InitStructure);
    USART_Enable(USART1, ENABLE);
}

// Retarget printf to USART1
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXDE) == RESET);
        USART_SendData(USART1, (uint8_t)ptr[i]);
    }
    return len;
}