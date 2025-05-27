/************************************************
* @file    sysclk.h 
* @author  APBashara
* @date    10/2024
* 
* @brief   Clock Configuration Prototypes
***********************************************/

#include "sysclk.h"

void SystemClock_Config() {
    // Set waitstaits and enable prefetch buffer
    FLASH->ACR |= (0x1 << FLASH_ACR_LATENCY_Pos);
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    // Enable HSI48
    RCC->CR2 |= RCC_CR2_HSI48ON;
    while(!(RCC->CR2 & RCC_CR2_HSI48RDY));

    // Configure Prescalers and set source to HSI48
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE_DIV1 | RCC_CFGR_SW_HSI48;

    // Update Clock Variable
    SystemCoreClockUpdate();
}