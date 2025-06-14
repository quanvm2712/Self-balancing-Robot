/*
 * MAX7219_1.h
 *
 *  Created on: May 18, 2025
 *      Author: quanvm198
 */

#ifndef INC_MAX7219_H_
#define INC_MAX7219_H_

#include "stm32f407xx.h"

/**
 * MAX7221 register definition
 */
#define MAX7219_REG_NOOP         0x00
#define MAX7219_REG_DIGIT0       0x01
#define MAX7219_REG_DIGIT1       0x02
#define MAX7219_REG_DIGIT2       0x03
#define MAX7219_REG_DIGIT3       0x04
#define MAX7219_REG_DIGIT4       0x05
#define MAX7219_REG_DIGIT5       0x06
#define MAX7219_REG_DIGIT6       0x07
#define MAX7219_REG_DIGIT7       0x08
#define MAX7219_REG_DECODE_MODE  0x09
#define MAX7219_REG_INTENSITY    0x0A
#define MAX7219_REG_SCAN_LIMIT   0x0B
#define MAX7219_REG_SHUTDOWN     0x0C
#define MAX7219_REG_DISPLAY_TEST 0x0F


/**
 * @brief MAX7219 Shutdown register values
 */
#define MAX7219_SHUTDOWN_MODE     0x00  /**< Shutdown mode: display off, oscillator stopped */
#define MAX7219_NORMAL_OPERATION  0x01  /**< Normal operation mode: display on, oscillator running */

/**
 * @brief MAX7219 Decode Mode register values
 */
#define MAX7219_NO_DECODE         0x00  /**< No decode for any digit (useful for 8x8 matrix) */
#define MAX7219_DECODE_DIGIT_0    0x01  /**< Code B decode for digit 0 only */
#define MAX7219_DECODE_DIGIT_0_3  0x0F  /**< Code B decode for digits 0 to 3 */
#define MAX7219_DECODE_ALL        0xFF  /**< Code B decode for digits 0 to 7 */

/**
 * @brief MAX7219 Intensity levels (brightness control)
 * Values correspond to duty cycle levels from dimmest (1/32) to brightest (31/32).
 */
#define MAX7219_INTENSITY_1_32    0x00  /**< 1/32 duty cycle (dimmest) */
#define MAX7219_INTENSITY_3_32    0x01  /**< 3/32 duty cycle */
#define MAX7219_INTENSITY_5_32    0x02  /**< 5/32 duty cycle */
#define MAX7219_INTENSITY_7_32    0x03  /**< 7/32 duty cycle */
#define MAX7219_INTENSITY_9_32    0x04  /**< 9/32 duty cycle */
#define MAX7219_INTENSITY_11_32   0x05  /**< 11/32 duty cycle */
#define MAX7219_INTENSITY_13_32   0x06  /**< 13/32 duty cycle */
#define MAX7219_INTENSITY_15_32   0x07  /**< 15/32 duty cycle */
#define MAX7219_INTENSITY_17_32   0x08  /**< 17/32 duty cycle */
#define MAX7219_INTENSITY_19_32   0x09  /**< 19/32 duty cycle */
#define MAX7219_INTENSITY_21_32   0x0A  /**< 21/32 duty cycle */
#define MAX7219_INTENSITY_23_32   0x0B  /**< 23/32 duty cycle */
#define MAX7219_INTENSITY_25_32   0x0C  /**< 25/32 duty cycle */
#define MAX7219_INTENSITY_27_32   0x0D  /**< 27/32 duty cycle */
#define MAX7219_INTENSITY_29_32   0x0E  /**< 29/32 duty cycle */
#define MAX7219_INTENSITY_31_32   0x0F  /**< 31/32 duty cycle (brightest) */

/**
 * @brief MAX7219 Scan-Limit register values
 * These values determine how many digits (0–7) are displayed.
 */
#define MAX7219_SCAN_DIGIT_0      0x00  /**< Display only digit 0 */
#define MAX7219_SCAN_DIGIT_0_1    0x01  /**< Display digits 0 and 1 */
#define MAX7219_SCAN_DIGIT_0_2    0x02  /**< Display digits 0 to 2 */
#define MAX7219_SCAN_DIGIT_0_3    0x03  /**< Display digits 0 to 3 */
#define MAX7219_SCAN_DIGIT_0_4    0x04  /**< Display digits 0 to 4 */
#define MAX7219_SCAN_DIGIT_0_5    0x05  /**< Display digits 0 to 5 */
#define MAX7219_SCAN_DIGIT_0_6    0x06  /**< Display digits 0 to 6 */
#define MAX7219_SCAN_DIGIT_0_7    0x07  /**< Display digits 0 to 7 (default) */


void MAX7219_SendCommand(SPI_RegDef_t* SPIx, uint8_t Address, uint8_t Data);
void MAX7219_SetDigitValue(SPI_RegDef_t* SPIx, uint8_t Digit, uint8_t Value);
void MAX7219_Init(SPI_RegDef_t* SPIx, uint8_t DecodeMode, uint8_t IntensityLevel, uint8_t ScanLimit);
void MAX7219_TestLED(SPI_RegDef_t* SPIx, _Bool IsEnabled);
void MAX7219_Clean(SPI_RegDef_t* SPIx);
void MAX7219_DisplayNumbers(SPI_RegDef_t* SPIx, uint8_t Position, uint32_t Number, uint8_t NoOfLEDDigits);
void MAX7219_LeftSignal(SPI_RegDef_t* SPIx);
void MAX7219_RightSignal(SPI_RegDef_t* SPIx);
void MAX7219_StopSignal(SPI_RegDef_t* SPIx);

#endif /* INC_MAX7219_H_ */
