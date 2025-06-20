/*
 * SR05.c
 *
 *  Created on: May 18, 2025
 *      Author: quanvm198
 */


#include "SR05.h"
#include "stm32f407xx.h"

/**
  * @brief  Reads distance data from SR05 ultrasonic sensor via USART.
  * @param  pUSART: Pointer to the USART handle structure used for communication.
  * @retval Distance value in centimeters if a valid frame is received;
  *         returns 1 if a reserved invalid frame is detected (0xFF 0xAA 0xAA),
  *         or previous distance value if checksum fails.
  */
uint16_t SR05_ReadData(USART_HandleTypeDef* pUSART) {
    uint8_t pRxBuffer[4];
    uint8_t byte = 0;
    static uint16_t distance = 0;

    // Wait for 0xFF header
    for(uint16_t count = 0; count <= 5000; count++){
        USART_Receive(pUSART, &byte, 1);
        if (byte == 0xFF) {
            pRxBuffer[0] = byte;
            USART_Receive(pUSART, &pRxBuffer[1], 3);
            break;
        }
    }

    if(pRxBuffer[1] == 0xAA && pRxBuffer[2] == 0xAA) return 1;

    uint8_t sum = (pRxBuffer[0] + pRxBuffer[1] + pRxBuffer[2]) & 0xFF;

    if (sum == pRxBuffer[3]) {
        distance = ((uint16_t)pRxBuffer[1] << 8) | pRxBuffer[2];
    }

    return distance;
}


