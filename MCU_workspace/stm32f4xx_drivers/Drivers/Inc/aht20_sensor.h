/*
 * aht20_sensor.h
 *
 *  Created on: Feb 23, 2025
 *      Author: nhduong
 */

#ifndef INC_AHT20_SENSOR_H_
#define INC_AHT20_SENSOR_H_

#include "stm32f407xx.h"

#define AHT20_I2C_ADDRESS         0x38


// Function prototypes
void AHT20_Init();
void AHT20_ReadData();

#endif /* INC_AHT20_SENSOR_H_ */
