/************************************************
* @file    spi.h 
* @author  Ava Fischer
* @date    4/2025
* 
* @brief   Prototype Functions for SPI Driver
***********************************************/

#include "stm32f042x6.h"
#include <stdint.h>
#include <stddef.h>

/* Macros -------------------------------------------------------------------*/
/* Structs and Enums --------------------------------------------------------*/
typedef enum {
    SPI_ERROR,
    SPI_OK,
} SPI_Status;

/* Function Prototypes ------------------------------------------------------*/

void SPI1_Init();
SPI_Status SPI_Transmit(SPI_TypeDef* SPI, uint8_t* data, size_t len);
SPI_Status SPI_Receive(SPI_TypeDef* SPI, uint8_t* buf, size_t len);