/************************************************
* @file    gpio.h 
* @author  APBashara
* @date    3/2025
* 
* @brief   GPIO Driver Header file
***********************************************/

#include <stdint.h>
#include <stddef.h>
#include "stm32f042x6.h"

typedef enum {
    GPIO_OK,
    GPIO_Error
} GPIO_Status;

typedef enum {
    GPIO_Mode_Input,
    GPIO_Mode_Output,
    GPIO_Mode_Alternate,
    GPIO_Mode_Analog
} GPIO_Mode;

typedef enum {
    GPIO_Pull_NoPull,
    GPIO_Pull_Up,
    GPIO_Pull_Down
} GPIO_Pull;

typedef enum {
    GPIO_Speed_Low,
    GPIO_Speed_Medium,
    GPIO_Speed_High
} GPIO_Speed;

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    GPIO_Mode mode;
    GPIO_Pull pull;
    GPIO_Speed speed;
} GPIO_Instance;


/**
 * @brief Initializes the GPIO pin
 * 
 * @param GPIO_InitStruct Pointer to GPIO_InitTypeDef structure
 * @return GPIO_Status
 */
GPIO_Status GPIO_Init(GPIO_Instance *GPIO_InitStruct);

/**
 * @brief Sets the GPIO pin to high
 * 
 * @param [GPIO_TypeDef] Pointer to the GPIO port
 * @param [uint16_t] Pin number
 */
void GPIO_Set(GPIO_TypeDef *port, uint16_t pin);

/**
 * @brief Sets the GPIO pin to low
 * 
 * @param [GPIO_TypeDef] Pointer to the GPIO port
 * @param [uint16_t] Pin number
 */
void GPIO_Reset(GPIO_TypeDef *port, uint16_t pin);

/**
 * @brief Toggles the GPIO pin
 * 
 * @param [GPIO_TypeDef] Pointer to the GPIO port
 * @param [uint16_t] Pin number
 */
void GPIO_Toggle(GPIO_TypeDef *port, uint16_t pin);

/**
 * @brief Reads the GPIO pin state
 * 
 * @param [GPIO_TypeDef] Pointer to the GPIO port
 * @param [uint16_t] Pin number
 * @return uint8_t Pin state (0 or 1)
 */
uint8_t GPIO_Read(GPIO_TypeDef *port, uint16_t pin);
