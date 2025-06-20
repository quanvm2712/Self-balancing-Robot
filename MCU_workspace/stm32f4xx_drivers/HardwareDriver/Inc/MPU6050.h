/*
 * MPU6050.h
 *
 *  Created on: May 24, 2025
 *      Author: nhduong
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include "stm32f407xx.h"

#define MPU6050_ADDRESS			0x68
#define MPU6050_WHO_AM_I		0x75

#define MPU6050_REG_PWR_MGMT_1  	0x6B
#define MPU6050_REG_ACCEL_XOUT_H 	0x3B
#define MPU6050_REG_GYRO_XOUT_H  	0x43
#define MPU6050_REG_CONFIG      	0x1A
#define MPU6050_REG_GYRO_CONFIG 	0x1B
#define MPU6050_REG_ACCEL_CONFIG	0x1C
#define MPU6050_REG_INT_ENABLE 	 	0x38
#define MPU6050_REG_INT_STATUS 	 	0x3A

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} MPU6050_Data;

typedef struct {
	double accel_x_mps2;
	double accel_y_mps2;
	double accel_z_mps2;
	double gyro_x_dps;
	double gyro_y_dps;
	double gyro_z_dps;
} MPU6050_ConvertedData;

I2C_StatusTypeDef MPU6050_Init(I2C_HandleTypeDef *hi2c);
I2C_StatusTypeDef MPU6050_ReadData(I2C_HandleTypeDef *hi2c, MPU6050_Data *data);
I2C_StatusTypeDef MPU6050_CheckDevice(I2C_HandleTypeDef *hi2c);
void MPU6050_ConvertData(const MPU6050_Data *raw_data, MPU6050_ConvertedData *converted_data);
void MPU6050_CalibGyro(void);
double MPU6050_GetAngle(const MPU6050_ConvertedData *data);


#endif /* INC_MPU6050_H_ */
