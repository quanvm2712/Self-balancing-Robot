/*
 * stm32f407xx_rcc.h
 *
 *  Created on: Feb 12, 2025
 *      Author: nhduong
 */

#ifndef INC_STM32F407XX_RCC_H_
#define INC_STM32F407XX_RCC_H_

#include "stm32f407xx.h"

//This returns the APB1 clock value
uint32_t RCC_GetPCLK1_Value(void);

//This returns the APB2 clock value
uint32_t RCC_GetPCLK2_Value(void);


uint32_t  RCC_GetPLLOutputClock(void);

#endif /* INC_STM32F407XX_RCC_H_ */
