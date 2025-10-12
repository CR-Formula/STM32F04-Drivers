/************************************************
* @file    main.c 
* @author  Ava Fischer
* @date    4/2025
* 
* @brief   Test for the SPI Driver
***********************************************/

#include "stm32f042x6.h"
#include "spi.h"

#define DATA_LENGTH 10

/* Function Calls -----------------------------------------------------------*/
void main() {

    // Initialize SPI
    SPI1_Init();


    // Test 1: just try sending data
    uint8_t* sendData[10] = {0xA, 0xB, 0xA, 0xB, 0xA, 0xB, 0xA, 0xB, 0xA, 0xB};

    SPI_Status tx1Status = SPI_Transmit(SPI1, sendData, DATA_LENGTH);
    if (tx1Status == SPI_OK) {
        printf("Data transmitted successfully!\n");
    } else {
        printf("Transmission failed.\n");
        return 1;
    }

    // Test 2: tie MOSI and MISO together
    // uint8_t* sendData[10] = {0xC, 0xD, 0xC, 0xD, 0xC, 0xD, 0xC, 0xD, 0xC, 0xD};

    // SPI_Status tx2Status = SPI_Transmit(SPI1, sendData, DATA_LENGTH);
    // if (tx2Status == SPI_OK) {
    //     printf("Data transmitted successfully!\n");
    // } else {
    //     printf("Transmission failed.\n");
    //     return 1;
    // }

    // SPI_Status rxStatus = SPI_Receive(SPI1, rxData, BUFFER_SIZE);
    // if (rxStatus == SPI_OK) {

    //     printf("Data received successfully!\n");

    //     int match = 1;
    //     for (size_t i = 0; i < BUFFER_SIZE; i++) {

    //         if (rxData[i] != txData[i]) {
    //             match = 0;
    //             printf("Data mismatch at index %d: Sent = %02X, Received = %02X\n", i, txData[i], rxData[i]);
    //         }
    //     }

    //     if (match) {
    //         printf("Loopback test passed!\n");
    //     }

    // } else {
    //     printf("Failed to receive data.\n");
    //     return 1;
    // }

}