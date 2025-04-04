/************************************************
* @file    timer.h 
* @author  APBashara
* @date    4/2025
* 
* @brief   Timer Driver Header file
***********************************************/

#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>
#include <stddef.h>

#include "stm32f042x6.h"

typedef enum {
    TIM_OK,
    TIM_Error
} TIM_Status;

typedef enum {
    TIM_Mode_OnePulse,
    TIM_Mode_Continuous
} TIM_Mode;

typedef enum {
    TIM_Prescaler_1,
    TIM_Prescaler_2,
    TIM_Prescaler_4,
    TIM_Prescaler_8,
    TIM_Prescaler_16,
    TIM_Prescaler_32,
    TIM_Prescaler_64,
    TIM_Prescaler_128
} TIM_Prescaler;

typedef enum {
    TIM_Polarity_Rising,
    TIM_Polarity_Falling
} TIM_Polarity;

typedef enum {
    TIM_ClockSource_Internal,
    TIM_ClockSource_External
} TIM_ClockSource;

typedef enum {
    TIM_TriggerSource_None,
    TIM_TriggerSource_External
} TIM_TriggerSource;

typedef enum {
    TIM_Interrupt_None,
    TIM_Interrupt_Update,
    TIM_Interrupt_Compare,
    TIM_Interrupt_Trigger
} TIM_Interrupt;

typedef struct {
    TIM_TypeDef *TIMx;
    uint32_t prescaler;
    uint32_t period;
    TIM_Mode mode;
} TIM_Instance;

/**
 * @brief Initializes the Timer
 * 
 * @param TIM_Instance [TIM_Instance] Pointer to TIM_Instance structure
 * @return [TIM_Status] Status of the initialization
 */
TIM_Status TIM_Init(TIM_Instance *TIM_Instance);

/**
 * @brief Start the Timer
 * 
 * @param TIMx [TIM_TypeDef] Pointer to the Timer instance
 */
void TIM_Start(TIM_TypeDef *TIMx);

/**
 * @brief Stop the Timer
 * 
 * @param TIMx [TIM_TypeDef] Pointer to the Timer instance
 */
void TIM_Stop(TIM_TypeDef *TIMx);

/**
 * @brief Return the current count of the timer
 * 
 * @param TIMx [TIM_TypeDef] Pointer to the Timer instance
 * @return [uint32_t] Current count of the timer
 */
uint32_t Get_Timer_Count(TIM_TypeDef *TIMx);














#endif