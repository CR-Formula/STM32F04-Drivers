/************************************************
* @file    can.c 
* @author  APBashara
* @date    10/2024
* 
* @brief   CAN Driver Implementation for STM32F042x6
***********************************************/

#include <stddef.h>

#include "can.h"

CAN_State can_state = CAN_State_Sleep;

/**
 * @brief Find an empty CAN Transmit mailbox
 * 
 * @return [uint8_t] Index of Empty Mailbox 
 */
static uint8_t Get_Empty_Mailbox() {
    return (CAN->TSR & CAN_TSR_CODE_Msk) >> CAN_TSR_CODE_Pos;
}

CAN_Status CAN_Init() {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_CANEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;

    // Remap to PA11 and PA12
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_PA12_RMP;

    // Configure PA12 CAN Tx
    GPIOA->MODER &= ~GPIO_MODER_MODER12;
    GPIOA->MODER |= (0x2 << GPIO_MODER_MODER12_Pos); // Alternate Function
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_12; // Push-Pull
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR12; // No Pull-Up or Pull-Down
    GPIOA->AFR[1] |= (0x4 << GPIO_AFRH_AFSEL12_Pos);
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR12;
    GPIOA->OSPEEDR |= (0x2 << GPIO_OSPEEDR_OSPEEDR12_Pos); // High Speed

    // Configure PB8 CAN Rx
    GPIOB->MODER &= ~GPIO_MODER_MODER8;
    GPIOB->MODER |= (0x2 << GPIO_MODER_MODER8_Pos); // Alternate Function
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_8; // Push-Pull
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR8; // Clear Pull-Up and Pull-Down
    GPIOB->PUPDR |= (0x1 << GPIO_PUPDR_PUPDR8_Pos); // Pull-Up
    GPIOB->AFR[1] |= (0x4 << GPIO_AFRH_AFSEL8_Pos);
    GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR8;
    GPIOB->OSPEEDR |= (0x2 << GPIO_OSPEEDR_OSPEEDR8_Pos); // High Speed

    // Initialize the CAN Peripheral
    CAN->MCR |= CAN_MCR_RESET; // Reset CAN
    CAN->MCR &= ~CAN_MCR_SLEEP; // Exit Sleep Mode
    while (CAN->MSR & CAN_MSR_SLAK); // Wait until Sleep Mode is exited
    CAN->MCR |= CAN_MCR_INRQ; // Request Initialization Mode
    while (!(CAN->MSR & CAN_MSR_INAK)); // Wait until Initialization Mode is entered

    // Configure CAN Settings
    CAN->MCR &= ~CAN_MCR_TXFP & ~CAN_MCR_RFLM & ~CAN_MCR_TTCM 
                & ~CAN_MCR_ABOM & ~CAN_MCR_TXFP;
    CAN->MCR |= CAN_MCR_AWUM | CAN_MCR_NART;
    can_state = CAN_State_Initialization;

    // Set Baud Rate
    // http://www.bittiming.can-wiki.info/
    CAN->BTR &= ~CAN_BTR_SILM & ~CAN_BTR_LBKM;
    CAN->BTR &= ~CAN_BTR_SJW & ~CAN_BTR_TS1 
                & ~CAN_BTR_TS2 & ~CAN_BTR_BRP;
    CAN->BTR |= (0xC << CAN_BTR_TS1_Pos) | (0x1 << CAN_BTR_TS2_Pos) 
                | (0x5 << CAN_BTR_BRP_Pos) | (0x0 << CAN_BTR_SJW_Pos);

    return CAN_OK;
}

CAN_Status CAN_Filters_Init() {
    if (can_state != CAN_State_Initialization) {
        return CAN_Error;
    }

    CAN->FMR |= CAN_FMR_FINIT; // Enter Filter Initialization Mode
    CAN->FMR &= ~CAN_FMR_CAN2SB_Msk; 
    CAN->FMR |= (28UL << CAN_FMR_CAN2SB_Pos); // Set Filter Start Bank to 0

    CAN->FM1R &= ~CAN_FM1R_FBM_Msk; // Set Filter 0 to Mask Mode
    CAN->FS1R &= ~CAN_FS1R_FSC_Msk; // Set Filter 0 to 32-bit Scale
    CAN->FFA1R &= ~CAN_FFA1R_FFA_Msk; // Set Filter 0 to FIFO 0

    CAN->sFilterRegister[0].FR1 = 0x0UL; // Set Filter 0 ID to 0
    CAN->sFilterRegister[0].FR2 = 0x0UL; // Set Filter 0 Mask to 0

    CAN->FA1R |= CAN_FA1R_FACT_Msk; // Enable Filter 0
    CAN->FMR &= ~CAN_FMR_FINIT; // Exit Filter Initialization Mode
    return CAN_OK;
}

CAN_Status CAN_Start() {
    if (can_state != CAN_State_Initialization) {
        return CAN_Error;
    }
    CAN->MCR &= ~CAN_MCR_INRQ; // Exit Initialization Mode
    while (CAN->MSR & CAN_MSR_INAK); // Wait until Normal Mode is entered
    can_state = CAN_State_Normal;

    return CAN_OK;
}

CAN_Status CAN_Stop() {
    if (can_state != CAN_State_Normal) {
        return CAN_Error;
    }
    CAN->MCR |= CAN_MCR_INRQ; // Enter Initialization Mode
    while (!(CAN->MSR & CAN_MSR_INAK)); // Wait until Initialization Mode is entered
    can_state = CAN_State_Initialization;
    return CAN_OK;
}

CAN_Status CAN_Transmit(CAN_Frame *frame) {
    // Basic Type Checking
    if (frame == NULL) { //  || can_state != CAN_State_Normal
        return CAN_Error;
    }

    volatile uint8_t mailbox = Get_Empty_Mailbox();

    // Clear the mailbox register
    CAN->sTxMailBox[mailbox].TIR = 0x0UL; 

    // Set ID, DLC, Frame Type, and Data
    CAN->sTxMailBox[mailbox].TIR |= (frame->id << CAN_TI0R_STID_Pos)
                                    | (frame->rtr << CAN_TI0R_RTR_Pos);
    CAN->sTxMailBox[mailbox].TDTR |= (frame->dlc << CAN_TDT0R_DLC_Pos);
    CAN->sTxMailBox[mailbox].TDLR = (frame->data[0] << CAN_TDL0R_DATA0_Pos) 
                                    | (frame->data[1] << CAN_TDL0R_DATA1_Pos)
                                    | (frame->data[2] << CAN_TDL0R_DATA2_Pos) 
                                    | (frame->data[3] << CAN_TDL0R_DATA3_Pos);
    CAN->sTxMailBox[mailbox].TDHR = (frame->data[4] << CAN_TDH0R_DATA4_Pos) 
                                    | (frame->data[5] << CAN_TDH0R_DATA5_Pos) 
                                    | (frame->data[6] << CAN_TDH0R_DATA6_Pos) 
                                    | (frame->data[7] << CAN_TDH0R_DATA7_Pos);
    // Request Transmission
    CAN->sTxMailBox[mailbox].TIR |= CAN_TI0R_TXRQ;
    return CAN_TX_Req;
}

CAN_Status CAN_Receive(CAN_Frame *frame) {
    if (frame == NULL || can_state != CAN_State_Normal) {
        return CAN_Error;
    }

    if ((CAN->RF0R & CAN_RF0R_FMP0)) {
        frame->id = (CAN->sFIFOMailBox[0].RIR & CAN_RI0R_STID_Msk) >> CAN_RI0R_STID_Pos;
        
        for (uint8_t i = 0; i < 4; i++) {
            frame->data[i] = (CAN->sFIFOMailBox[0].RDLR >> (i * 8)) & 0xFF;
        }
        for (uint8_t i = 0; i < 4; i++) {
            frame->data[i + 4] = (CAN->sFIFOMailBox[0].RDHR >> (i * 8)) & 0xFF;
        }
        CAN->RF0R |= CAN_RF0R_RFOM0; // Release FIFO 0
        return CAN_OK;
    }
    else if ((CAN->RF1R & CAN_RF1R_FMP1)) {
        frame->id = (CAN->sFIFOMailBox[1].RIR & CAN_RI1R_STID_Msk) >> CAN_RI1R_STID_Pos;

        for (uint8_t i = 0; i < 4; i++) {
            frame->data[i] = (CAN->sFIFOMailBox[1].RDLR >> (i * 8)) & 0xFF;
        }
        for (uint8_t i = 0; i < 4; i++) {
            frame->data[i + 4] = (CAN->sFIFOMailBox[1].RDHR >> (i * 8)) & 0xFF;
        }
        CAN->RF1R |= CAN_RF1R_RFOM1; // Release FIFO 1
        return CAN_OK;
    }
    else {
        return CAN_Fifo_Error;
    }
}