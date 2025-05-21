/*
 * SR05.h
 *
 *  Created on: May 18, 2025
 *      Author: quanvm198
 */

#ifndef INC_SR05_H_
#define INC_SR05_H_

#include "stm32f407xx.h"

uint16_t SR05_ReadData(USART_HandleTypeDef* pUSART);
void SR05_InitTriggerIO();

#endif /* INC_SR05_H_ */
