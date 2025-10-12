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

#define TIMEOUT 1000 // ???

void SPI1_Init(SPI_Mode* mode){

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Write proper GPIO registers: Configure GPIO for MOSI, MISO and SCK pins
    // See p.33 of the datasheet for the STM32F042F6P6
    GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    GPIOA->MODER |= (0x02 << GPIO_MODER_MODER5_Pos)
             | (0x02 << GPIO_MODER_MODER6_Pos) | (0x02 << GPIO_MODER_MODER7_Pos);

    GPIOA->AFR[0] |= (0x00 << GPIO_AFRL_AFSEL5_Pos) 
             | (0x00 << GPIO_AFRL_AFSEL6_Pos) | (0x00 << GPIO_AFRL_AFSEL7_Pos);

    GPIOB->MODER &= ~GPIO_MODER_MODER1;
    GPIOB->MODER |= (0x01 << GPIO_MODER_MODER1_Pos); // Set PB1 to output mode
                         
    // Clear PUPDR7 field then set it to 0b10
    GPIOA->PUPDR &= ~(0x3UL << GPIO_PUPDR_PUPDR7_Pos);
    GPIOA->PUPDR |=  (0x2UL << GPIO_PUPDR_PUPDR7_Pos);

    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR5_Msk);
    GPIOA->PUPDR |=  (0x2 << GPIO_PUPDR_PUPDR5_Pos);

    GPIOA->OSPEEDR |= (0x03 << GPIO_OSPEEDR_OSPEEDR4_Pos) | (0x03 << GPIO_OSPEEDR_OSPEEDR5_Pos)
    | (0x03 << GPIO_OSPEEDR_OSPEEDR6_Pos) | (0x03 << GPIO_OSPEEDR_OSPEEDR7_Pos);

    SPI1->CR1 &= ~(SPI_CR1_BIDIMODE | SPI_CR1_CRCEN | SPI_CR1_RXONLY |
    SPI_CR1_SSM | SPI_CR1_LSBFIRST | SPI_CR1_CPHA | SPI_CR1_CPOL);

    SPI1->CR1 &= ~SPI_CR1_CPOL;
    SPI1->CR1 |=  SPI_CR1_CPHA;

    SPI1->CR1 &= ~SPI_CR1_MSTR;
    SPI1->CR1 |= (0x5 << SPI_CR1_BR_Pos); // sysclock / 64

    SPI1->CR1 &= ~SPI_CR1_RXONLY;

    SPI1->CR1 |= SPI_CR1_SSM;
    
    if (mode == SPI_MODE_MASTER) {
        SPI1->CR1 |= SPI_CR1_MSTR;
        SPI1->CR1 |= SPI_CR1_SSI;        // NSS high
    } else {
        SPI1->CR1 &= ~SPI_CR1_MSTR;
        SPI1->CR1 &= ~SPI_CR1_SSI;       // NSS low — means selected
    }

    SPI1->CR2 &= ~SPI_CR2_FRF; // SPI Motorola Mode
    SPI1->CR2 &= ~SPI_CR2_SSOE;

    SPI1->CR2 &= ~SPI_CR2_DS_Msk;
    SPI1->CR2 |= (0x7 << SPI_CR2_DS_Pos);

    SPI1->CR2 |= SPI_CR2_FRXTH;

    //GPIOA->BSRR = GPIO_BSRR_BS_4;
    //GPIOB->BSRR = GPIO_BSRR_BS_1;  // Drive PB1 high after init
    //SPI1->CR1 |= SPI_CR1_SPE;
    Set_Pin(GPIOB, 1);
}

SPI_Status SPI_Transmit(SPI_TypeDef* SPI, uint8_t* data, size_t len){

    // null pointer checks
    if (SPI == NULL || data == NULL || len <= 0) {
      return SPI_ERROR;
    }

    // ensure configured data frame size isn't larger than the maximum (16 bits)
    if ((SPI->CR2 & SPI_CR2_DS_Msk) > (0xF << SPI_CR2_DS_Pos)) {
      return SPI_ERROR;
    }

    SPI->CR1 |= SPI_CR1_SPE; // Enable SPI

    // chip select
    //GPIOB->BSRR = GPIO_BSRR_BR_1;
    Set_Pin(GPIOB, 1);
  
    if ((SPI->CR2 & SPI_CR2_DS_Msk) == (0xF << SPI_CR2_DS_Pos)) { // 16-bit Data Frame
      while (len > 0) {

        uint32_t timeout = TIMEOUT;

        while (!(SPI->SR & SPI_SR_TXE)); 
        SPI->DR = *((uint16_t*)data);
        // increment data pointer by 2 bytes
        data += sizeof(uint16_t);
        len--;

        // timeout
        while (!(SPI->SR & SPI_SR_RXNE)){
          if (--timeout == 0) return SPI_ERROR;
        };

        (void)SPI->DR;
      }
    }
    else { // 8-bit Data Frame

      while (len > 0) {

        uint32_t timeout = TIMEOUT;

        while (!(SPI->SR & SPI_SR_TXE));
        SPI->DR = *data;
        //*((__IO uint8_t *)&SPI->DR) = *data;
        data++;
        len--;

        // timeout
        while (!(SPI->SR & SPI_SR_RXNE)){
          if (--timeout == 0) return SPI_ERROR;
        };

        (void)SPI->DR;
      }
    }

    uint32_t timeout = TIMEOUT;

    // Wait for last byte to be sent
    while (!(SPI->SR & SPI_SR_TXE)){
      // timeout
      if (--timeout == 0) return SPI_ERROR;
    };

    timeout = TIMEOUT;

    while (SPI->SR & SPI_SR_BSY){
      // timeout
      if (--timeout == 0) return SPI_ERROR;
    }; 

    // // Deselect chip
    //GPIOB->BSRR = GPIO_BSRR_BS_1;
    Clear_Pin(GPIOB, 1);

    SPI->CR1 &= ~SPI_CR1_SPE; // Disable SPI
  
    return SPI_OK;
}

SPI_Status SPI_Receive(SPI_TypeDef* SPI, uint8_t* buf, size_t len){

    // null pointer checks
    if (SPI == NULL || buf == NULL || len <= 0) {
      return SPI_ERROR;
    }

    // ensure configured data frame size isn't larger than the maximum (16 bits)
    if ((SPI->CR2 & SPI_CR2_DS_Msk) > (0xF << SPI_CR2_DS_Pos)) {
      return SPI_ERROR;
    }

    // chip select
    //GPIOB->BSRR = GPIO_BSRR_BR_1;
    Set_Pin(GPIOB, 1);
    
    if ((SPI->CR2 & SPI_CR2_DS_Msk) == (0xF << SPI_CR2_DS_Pos)) { // 16-bit Data Frame
      while (len > 0) {
        while(!(SPI->SR & SPI_SR_TXE)){

          // timeout
          uint32_t timeout = TIMEOUT;
          if (--timeout == 0) return SPI_ERROR; 
        };
        SPI->DR = 0x0000; // Dummy Data
        while (!(SPI->SR & SPI_SR_RXNE)){

          // timeout
          uint32_t timeout = TIMEOUT;
          if (--timeout == 0) return SPI_ERROR; 
        };
        *((uint16_t*)buf) = SPI->DR; // Cast to 16-bit

        if (SPI->SR & SPI_SR_OVR) {
          (void)SPI->DR;
          return SPI_ERROR;
        }
          
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

        if (SPI->SR & SPI_SR_OVR) {
          (void)SPI->DR;
          return SPI_ERROR;
        }
          
      }
    }

    // Wait for last byte to be received
    uint32_t timeout = TIMEOUT;
    while(SPI->SR & SPI_SR_RXNE){
      if (--timeout == 0) return SPI_ERROR;
    };

    timeout = TIMEOUT;
    while (SPI->SR & SPI_SR_BSY){
      if (--timeout == 0) return SPI_ERROR;
    };
  
    // Deselect chip
    //GPIOB->BSRR = GPIO_BSRR_BS_1;
    Clear_Pin(GPIOB, 1);
  
    return SPI_OK;
}