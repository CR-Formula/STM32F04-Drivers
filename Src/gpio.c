/************************************************
* @file    gpio.c
* @author  APBashara
* @date    3/2025
* 
* @brief   GPIO Driver Implementation for STM32F042x6
***********************************************/

#include "gpio.h"
#include "stm32f042x6.h"

GPIO_Status GPIO_Init(GPIO_Instance *GPIO_Instance) {   
    if (GPIO_Instance == NULL) {
        return GPIO_Error;
    }

    // Configure the mode
    GPIO_Instance->port->MODER &= ~(0x3 << (GPIO_Instance->pin * 2));
    GPIO_Instance->port->MODER |= (GPIO_Instance->mode << (GPIO_Instance->pin * 2));

    // Configure the pull-up/pull-down resistors
    GPIO_Instance->port->PUPDR &= ~(0x3 << (GPIO_Instance->pin * 2));
    GPIO_Instance->port->PUPDR |= (GPIO_Instance->pull << (GPIO_Instance->pin * 2));

    // Configure the speed
    GPIO_Instance->port->OSPEEDR &= ~(0x3 << (GPIO_Instance->pin * 2));
    GPIO_Instance->port->OSPEEDR |= (GPIO_Instance->speed << (GPIO_Instance->pin * 2));

    return GPIO_OK;
}

void GPIO_Set(GPIO_TypeDef *port, uint16_t pin) {
    if (port == NULL || pin > 15) {
        return;
    }

    // Set the pin high
    port->ODR |= (1 << pin);
}

void GPIO_Reset(GPIO_TypeDef *port, uint16_t pin) {
    if (port == NULL || pin > 15) {
        return;
    }

    // Set the pin low
    port->ODR &= ~(1 << pin);
}

void GPIO_Toggle(GPIO_TypeDef *port, uint16_t pin) {
    if (port == NULL || pin > 15) {
        return;
    }

    // Toggle the pin state
    port->ODR ^= (1 << pin);
}
