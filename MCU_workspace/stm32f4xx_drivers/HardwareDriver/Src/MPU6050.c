/*
 * MPU6050.c
 *
 *  Created on: May 24, 2025
 *      Author: nhduong
 */

#include "MPU6050.h"

extern I2C_HandleTypeDef hi2c1;
extern MPU6050_Data sensor_data;
extern MPU6050_ConvertedData converted_data;
double MPU_CalibValue = 0.0;
uint16_t data_count = 0;

/**
  * @brief  Write a register on MPU6050
  * @param  hi2c: Pointer to I2C_HandleTypeDef structure
  * @param  reg: Register address
  * @param  value: Value to write
  * @retval I2C_StatusTypeDef: Status of the operation
  */
static I2C_StatusTypeDef MPU6050_WriteRegister(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    return I2C_Master_Transmit(hi2c, data, 2, MPU6050_ADDRESS, I2C_SR_DISABLE);
}

/**
  * @brief  Read a register on MPU6050
  * @param  hi2c: Pointer to I2C_HandleTypeDef structure
  * @param  reg: Register address
  * @param  data: Pointer to data buffer
  * @param  size: Number of bytes to read
  * @retval I2C_StatusTypeDef: Status of the operation
  */
static I2C_StatusTypeDef MPU6050_ReadRegister(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *data, uint16_t size) {
	return I2C_Mem_Read(hi2c, MPU6050_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, size);
}

/**
  * @brief  Initialize MPU6050 sensor
  * @param  hi2c: Pointer to I2C_HandleTypeDef structure
  * @retval I2C_StatusTypeDef: Status of the operation
  */
I2C_StatusTypeDef MPU6050_Init(I2C_HandleTypeDef *hi2c) {
    I2C_StatusTypeDef status;

   // check connect
    status = MPU6050_CheckDevice(hi2c);
    if (status != I2C_OK) {
        return status;
    }

    /* wake up */
    status = MPU6050_WriteRegister(hi2c, MPU6050_REG_PWR_MGMT_1, 0x00);
    if (status != I2C_OK) {
        return status;
    }

    /* Digital filter configuration (DLPF)) */
    status = MPU6050_WriteRegister(hi2c, MPU6050_REG_CONFIG, 0x03); // DLPF = 3 (44Hz)
    if (status != I2C_OK) {
        return status;
    }

    /* Gyroscope scale configuration  */
    status = MPU6050_WriteRegister(hi2c, MPU6050_REG_GYRO_CONFIG, 0x00);
    if (status != I2C_OK) {
        return status;
    }

    /* Accelerometer scale configuration */
    status = MPU6050_WriteRegister(hi2c, MPU6050_REG_ACCEL_CONFIG, 0x00);
    if (status != I2C_OK) {
        return status;
    }


    return I2C_OK;
}

/**
  * @brief  Check MPU6050 device connection
  * @param  hi2c: Pointer to I2C_HandleTypeDef structure
  * @retval I2C_StatusTypeDef: Status of the operation
  */
I2C_StatusTypeDef MPU6050_CheckDevice(I2C_HandleTypeDef *hi2c) {
    uint8_t who_am_i;
    I2C_StatusTypeDef status = MPU6050_ReadRegister(hi2c, MPU6050_WHO_AM_I, &who_am_i, 1);
    if (status != I2C_OK || who_am_i != 0x68) {
        return I2C_ERROR;
    }
    return I2C_OK;
}

/**
  * @brief  Read raw data from MPU6050
  * @param  hi2c: Pointer to I2C_HandleTypeDef structure
  * @param  data: Pointer to MPU6050_Data structure
  * @retval I2C_StatusTypeDef: Status of the operation
  */
I2C_StatusTypeDef MPU6050_ReadData(I2C_HandleTypeDef *hi2c, MPU6050_Data *data) {
    uint8_t buffer[14];
    I2C_StatusTypeDef status;

    /* Read 14 bytes of data from the ACCEL_XOUT_H register */
    status = MPU6050_ReadRegister(hi2c, MPU6050_REG_ACCEL_XOUT_H, buffer, 14);
    if (status != I2C_OK) {
        return status;
    }

    // data of accel
    data->accel_x = (int16_t)((buffer[0] << 8) | buffer[1]);
    data->accel_y = (int16_t)((buffer[2] << 8) | buffer[3]);
    data->accel_z = (int16_t)((buffer[4] << 8) | buffer[5]);

    // data of gyro
    data->gyro_x = (int16_t)((buffer[8] << 8) | buffer[9]);
    data->gyro_y = (int16_t)((buffer[10] << 8) | buffer[11]);
    data->gyro_z = (int16_t)((buffer[12] << 8) | buffer[13]);

    return I2C_OK;
}


/**
  * @brief  Calibrates the gyroscope bias on the X-axis of the MPU6050 sensor.
  * @param  None
  * @retval None
  */
void MPU6050_CalibGyro(void)
{
  for(uint16_t count = 0; count < 1000; count++){
      if(MPU6050_ReadData(&hi2c1, &sensor_data) == I2C_OK){
        MPU6050_ConvertData(&sensor_data, &converted_data);
        MPU_CalibValue += converted_data.gyro_x_dps;
        data_count ++;
      }
  }

  MPU_CalibValue /= data_count;
}

/**
  * @brief  Convert raw data to physical units
  * @param  raw_data: Pointer to MPU6050_Data structure
  * @param  converted_data: Pointer to MPU6050_ConvertedData structure
  * @retval None
  */
void MPU6050_ConvertData(const MPU6050_Data *raw_data, MPU6050_ConvertedData *converted_data) {

    converted_data->accel_x_mps2 = (double)raw_data->accel_x * 9.81f / 16384.0f;
    converted_data->accel_y_mps2 = (double)raw_data->accel_y * 9.81f / 16384.0f;
    converted_data->accel_z_mps2 = (double)raw_data->accel_z * 9.81f / 16384.0f;

    converted_data->gyro_x_dps = (double)raw_data->gyro_x / 131.0f;
    converted_data->gyro_y_dps = (double)raw_data->gyro_y / 131.0f;
    converted_data->gyro_z_dps = (double)raw_data->gyro_z / 131.0f;

}

/**
 * @brief Calculate angle using complementary filter
 * @param data: Pointer to MPU6050_ConvertedData structure
 * @param dt: Time step (seconds)
 * @return Current angle (degrees)
 */
double MPU6050_GetAngle(const MPU6050_ConvertedData *data) {
    // static double prev_pitch_gyro = 0.0;
    double acc_x = data->accel_x_mps2;
    double acc_y = data->accel_y_mps2;
    double acc_z = data->accel_z_mps2;

    // Complementary filter
    double alpha = 0.02;
    //Last tick
    static uint32_t lastTick = 0;
    //Previous gyro pitch data
    static double prev_pitch_gyro = 0.0;

    //Get pitch data from Accelerometer
    double pitch_acc = atan2f(acc_y, sqrtf(acc_x * acc_x + acc_z * acc_z)) * 180.0 / M_PI;

    //Calculate dt
    uint32_t currentTick = getTick();
    double dt = (currentTick - lastTick) / 1000.0;
    lastTick = currentTick;

    //Calculate Gyro pitch data use Complementary filter
    double current_pitch_gyro = prev_pitch_gyro + (data->gyro_x_dps - MPU_CalibValue) * dt;
    double angle = (1.0 - alpha) * current_pitch_gyro + alpha * pitch_acc;
    prev_pitch_gyro = angle;

    return angle;
}
