/************************************************
* @file    sysclk.h 
* @author  APBashara
* @date    10/2024
* 
* @brief   Clock Configuration Prototypes
***********************************************/

#include "stm32f042x6.h"

/**
 * @brief Configure the System Clock to 48MHz
 * 
 * @note Uses the HSI48 through the PLL
 */
void Sysclock_48();