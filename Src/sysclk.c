/************************************************
* @file    sysclk.h 
* @author  APBashara
* @date    10/2024
* 
* @brief   Clock Configuration Prototypes
***********************************************/

#include "sysclk.h"

void Sysclock_48() {
    // Set waitstaits and enable prefetch buffer
    FLASH->ACR |= (0x1 << FLASH_ACR_LATENCY_Pos);
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    // Enable HSI48
    RCC->CR2 |= RCC_CR2_HSI48ON;
    while(!(RCC->CR2 & RCC_CR2_HSI48RDY));

    // Configure PLL and MCO
    RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV2;
    // Set APB/AHB Prescaler, MCO, PLLMUL, and PLLSRC
    RCC->CFGR |= RCC_CFGR_PPRE_DIV1 | RCC_CFGR_HPRE_DIV1 | RCC_CFGR_MCO_PLL |
                RCC_CFGR_PLLMUL_2 | RCC_CFGR_PLLNODIV | RCC_CFGR_PLLSRC_HSI_PREDIV;

    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));

    // Set PLL as system clock
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));

    // Update Clock Variable
    SystemCoreClockUpdate();
}