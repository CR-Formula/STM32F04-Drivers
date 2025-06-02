/************************************************
* @file    spi.c 
* @author  Ava Fischer
* @date    4/2025
* 
* @brief   SPI Driver Implementation
*
* 4-11: Feeling okay about the initialization; Transmit and Receive definitely still a WIP
***********************************************/

#include "spi.h"

void SPI1_Init(){

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_SPI1EN;

    // Write proper GPIO registers: Configure GPIO for MOSI, MISO and SCK pins
    // See p.33 of the datasheet for the STM32F042F6P6
    GPIOA->MODER &= ~GPIO_MODER_MODE4 & ~GPIO_MODER_MODE5 & ~GPIO_MODER_MODE6 & ~GPIO_MODER_MODE7;
    GPIOA->MODER |= (0x02 << GPIO_MODER_MODE4_Pos) | (0x02 << GPIO_MODER_MODE5_Pos)
             | (0x02 << GPIO_MODER_MODE6_Pos) | (0x02 << GPIO_MODER_MODE7_Pos);

    GPIOA->AFR[0] |= (0x00 << GPIO_AFRH_AFSEL4_Pos) | (0x00 << GPIO_AFRH_AFSEL5_Pos) 
              | (0x00 << GPIO_AFRH_AFSEL6_Pos) | (0x00 << GPIO_AFRH_AFSEL7_Pos);

    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD7;
    GPIOA->PUPDR |= (0x2 << GPIO_PUPDR_PUPD7);

    GPIOA->OSPEEDR |= (0x03 << GPIO_OSPEEDR_OSPEED4_Pos) | (0x03 << GPIO_OSPEEDR_OSPEED5_Pos)
    | (0x03 << GPIO_OSPEEDR_OSPEED6_Pos) | (0x03 << GPIO_OSPEEDR_OSPEED7_Pos);

    SPI1->CR1 &= ~SPI_CR1_BIDIMODE & ~SPI_CR1_CRCEN & ~SPI_CR1_DFF & ~SPI_CR1_RXONLY
    & ~SPI_CR1_SSM & ~SPI_CR1_LSBFIRST & ~SPI_CR1_CPHA & ~SPI_CR1_CPOL;

    SPI1->CR1 &= ~SPI_CR1_MSTR;
    SPI1->CR1 |= (0x5 << SPI_CR1_BR_Pos); // sysclock / 64

    SPI1->CR2 &= ~SPI_CR2_FRF; // SPI Motorola Mode
    SPI1->CR2 |= SPI_CR2_SSOE; 

    GPIOA->BSRR = GPIO_BSRR_BS4;
}

SPI_Status SPI_Transmit(SPI_TypeDef* SPI, uint8_t* data, size_t len){

    SPI->CR1 |= SPI_CR1_SPE; // Enable SPI
    if ((SPI->CR2 & SPI_CR2_DS_Msk) == (0xF << SPI_CR2_DS_Pos)) { // 16-bit Data Frame
      while (len > 0) {
        while (!(SPI->SR & SPI_SR_TXE)); 
        SPI->DR = *((uint16_t*)data);
        // increment data pointer by 2 bytes
        data += sizeof(uint16_t);
        len--;
        while (!(SPI->SR & SPI_SR_RXNE));
        (void)SPI->DR;
      }
    }
    else { // 8-bit Data Frame
      while (len > 0) {
        while (!(SPI->SR & SPI_SR_TXE));
        SPI->DR = *data;
        data++;
        len--;
        while (!(SPI->SR & SPI_SR_RXNE));
        (void)SPI->DR;
      }
    }
    // Wait for last byte to be sent
    while (!(SPI->SR & SPI_SR_TXE));
    while (SPI->SR & SPI_SR_BSY);
  
    SPI->CR1 &= ~SPI_CR1_SPE; // Disable SPI
  
    return SPI_OK;
}

SPI_Status SPI_Receive(SPI_TypeDef* SPI, uint8_t* buf, size_t len){

    SPI->CR1 |= SPI_CR1_SPE; // Enable SPI
    
    if ((SPI->CR2 & SPI_CR2_DS_Msk) == (0xF << SPI_CR2_DS_Pos)) { // 16-bit Data Frame
      while (len > 0) {
        while(!(SPI->SR & SPI_SR_TXE));
        SPI->DR = 0x0000; // Dummy Data
        while (!(SPI->SR & SPI_SR_RXNE));
        *((uint16_t*)buf) = SPI->DR; // Cast to 16-bit
        // increment data pointer by 2 bytes and len by 2
        buf += sizeof(uint16_t);
        len --;
      }
    } else { // 8-bit Data Frame
      for (size_t i = 0; i < len; i++) {
        while(!(SPI->SR & SPI_SR_TXE));
        SPI->DR = 0x00; // Dummy Data to generate clk
        while (!(SPI->SR & SPI_SR_RXNE));
        buf[i] = SPI->DR;
      }
    }

    // Wait for last byte to be received
    while(SPI->SR & SPI_SR_RXNE);
    while (SPI->SR & SPI_SR_BSY);
  
    SPI->CR1 &= ~SPI_CR1_SPE; // Disable SPI
  
    return SPI_OK;
}