/************************************************
* @file    timer.c
* @author  APBashara
* @date    4/2025
* 
* @brief   Timer Driver Source file
***********************************************/

#include "timer.h"
#include "stm32f042x6.h"

TIM_Status TIM_Init(TIM_Instance *TIM_Instance) {
    if (TIM_Instance == NULL) {
        return TIM_Error;
    }

    // Enable the timer clock
    if (TIM_Instance == TIM1) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    } else if (TIM_Instance == TIM2) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    } else {
        return TIM_Error;
    }

    // Configure the prescaler
    TIM_Instance->TIMx->PSC = TIM_Instance->prescaler - 1;

    // Configure the auto-reload register
    TIM_Instance->TIMx->ARR = TIM_Instance->period - 1;

    // Set the timer mode
    if (TIM_Instance->mode == TIM_Mode_OnePulse) {
        TIM_Instance->TIMx->CR1 |= TIM_CR1_OPM;
    } else {
        TIM_Instance->TIMx->CR1 &= ~TIM_CR1_OPM;
    }

    // Enable the timer
    TIM_Instance->TIMx->CR1 |= TIM_CR1_CEN;

    return TIM_OK;
}

TIM_Status TIM_Start(TIM_Instance *TIM_Instance) {
    if (TIM_Instance == NULL) {
        return TIM_Error;
    }

    // Start the timer
    TIM_Instance->TIMx->CR1 |= TIM_CR1_CEN;

    return TIM_OK;
}

TIM_Status TIM_Stop(TIM_Instance *TIM_Instance) {
    if (TIM_Instance == NULL) {
        return TIM_Error;
    }

    // Stop the timer
    TIM_Instance->TIMx->CR1 &= ~TIM_CR1_CEN;

    return TIM_OK;
}

uint32_t TIM_GetCounter(TIM_Instance *TIM_Instance) {
    if (TIM_Instance == NULL) {
        return 0;
    }

    // Get the current counter value
    return TIM_Instance->TIMx->CNT;
}