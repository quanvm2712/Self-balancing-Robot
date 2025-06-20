/*
 * MAX7219.c
 *
 *  Created on: May 18, 2025
 *      Author: quanvm198
 */
#include "MAX7219.h"


/**
  * @brief  Sends a command (address and data) to the MAX7219 via SPI.
  * @param  SPIx: Pointer to the SPI peripheral configuration structure.
  * @param  Address: Register address in MAX7219.
  * @param  Data: Data to be sent to the specified address.
  * @retval None
  */

void MAX7219_SendCommand(SPI_RegDef_t* SPIx, uint8_t Address, uint8_t Data){
  uint8_t TX_Data[2] = {Address, Data};

  SPI_Transmit(SPIx, TX_Data, 2);
}

/**
  * @brief  Sets the operation mode (shutdown or normal) of the MAX7219.
  * @param  SPIx: Pointer to the SPI peripheral configuration structure.
  * @param  Mode: Operation mode. Use 0 for shutdown, 1 for normal operation.
  * @retval None
  */
void MAX7219_OperationMode(SPI_RegDef_t* SPIx, _Bool Mode){
  MAX7219_SendCommand(SPIx, MAX7219_REG_SHUTDOWN, Mode);
}

/**
  * @brief  Configures the decode mode for the MAX7219 (BCD decode or no decode).
  * @param  SPIx: Pointer to the SPI peripheral configuration structure.
  * @param  DecodeMode: Decode mode setting (0x00 to 0xFF depending on digits).
  * @retval None
  */
void MAX7219_SetDecodeMode(SPI_RegDef_t* SPIx, uint8_t DecodeMode){
  MAX7219_SendCommand(SPIx, MAX7219_REG_DECODE_MODE, DecodeMode);
}

/**
  * @brief  Sets the brightness intensity level of the MAX7219 display.
  * @param  SPIx: Pointer to the SPI peripheral configuration structure.
  * @param  IntensityLevel: Intensity level (0x00 to 0x0F).
  * @retval None
  */
void MAX7219_SetIntensity(SPI_RegDef_t* SPIx, uint8_t IntensityLevel){
  MAX7219_SendCommand(SPIx, MAX7219_REG_INTENSITY, IntensityLevel);
}

/**
  * @brief  Sets the scan limit for number of digits to be displayed.
  * @param  SPIx: Pointer to the SPI peripheral configuration structure.
  * @param  ScanLimit: Number of digits (0–7).
  * @retval None
  */
void MAX7219_SetScanLimit(SPI_RegDef_t* SPIx, uint8_t ScanLimit){
  MAX7219_SendCommand(SPIx, MAX7219_REG_SCAN_LIMIT, ScanLimit);
}

/**
  * @brief  Initializes the MAX7219 display with basic configurations.
  * @param  SPIx: Pointer to the SPI peripheral configuration structure.
  * @param  DecodeMode: Decode mode configuration.
  * @param  IntensityLevel: Brightness level.
  * @param  ScanLimit: Number of digits to scan/display.
  * @retval None
  */
void MAX7219_Init(SPI_RegDef_t* SPIx, uint8_t DecodeMode, uint8_t IntensityLevel, uint8_t ScanLimit){
  MAX7219_Clean(SPIx);
  MAX7219_OperationMode(SPIx, MAX7219_NORMAL_OPERATION);
  MAX7219_SetDecodeMode(SPIx, DecodeMode);
  MAX7219_SetIntensity(SPIx, IntensityLevel);
  MAX7219_SetScanLimit(SPIx, ScanLimit);
}

/**
  * @brief  Displays a value on a specific digit of the MAX7219.
  * @param  SPIx: Pointer to the SPI peripheral configuration structure.
  * @param  Digit: Digit register (0x01 to 0x08).
  * @param  Value: Value to be displayed on the digit.
  * @retval None
  */
void MAX7219_SetDigitValue(SPI_RegDef_t* SPIx, uint8_t Digit, uint8_t Value){
  MAX7219_SendCommand(SPIx, Digit, Value);
}

/**
  * @brief  Enables or disables display test mode.
  * @param  SPIx: Pointer to the SPI peripheral configuration structure.
  * @param  IsEnabled: Set to 1 to enable test mode, 0 to disable.
  * @retval None
  */
void MAX7219_TestLED(SPI_RegDef_t* SPIx, _Bool IsEnabled){
  MAX7219_SendCommand(SPIx, MAX7219_REG_DISPLAY_TEST, IsEnabled);
}

/**
  * @brief  Clears all digits on the display.
  * @param  SPIx: Pointer to the SPI peripheral configuration structure.
  * @retval None
  */
void MAX7219_Clean(SPI_RegDef_t* SPIx){
  for(int count = 0; count <= 0xC; count++){
      MAX7219_SendCommand(SPIx, count, 0);
  }
  MAX7219_SendCommand(SPIx, 0xF, 0);
}

/**
  * @brief  Calculates 10 raised to the power of x.
  * @param  x: The exponent value.
  * @retval 10^x as an unsigned 32-bit integer.
  */
uint32_t POWER_10(uint8_t x){
  double result = 1;
  while (x--) {
      result *= 10;
  }
  return result;
}

/**
  * @brief  Displays a number starting from a specific position on the MAX7219.
  * @param  SPIx: Pointer to the SPI peripheral configuration structure.
  * @param  Position: Starting digit position (1 to 8).
  * @param  Number: The number to be displayed.
  * @param  NoOfLEDDigits: Number of digits to be shown (padding with zero if needed).
  * @retval None
  */
void MAX7219_DisplayNumbers(SPI_RegDef_t* SPIx, uint8_t Position, uint32_t Number, uint8_t NoOfLEDDigits){
//  //Get number of digits of the number
//  uint8_t NumberOfDigits = 0;
//  uint32_t Temp_Number = Number;
//  while(Temp_Number >= 0){
//      NumberOfDigits += 1;
//      Temp_Number /= 10;
//      if(!Temp_Number) break;
//  }
//
//  //Display each numeber's digit
//  uint32_t Pow_Number = POWER_10(NumberOfDigits - 1);
//  Temp_Number = Number;
//  uint8_t digit = 0;
//
//  while(Pow_Number){
//      digit = Temp_Number / Pow_Number;  //Get 1st digit in the temporary number
//      MAX7219_SendCommand(SPIx, StartDigit, digit); //Display the digit in desired position
//      StartDigit--;  //Update the next position to display
//      Temp_Number %= Pow_Number;  //Get the number's next digit
//      Pow_Number /= 10;
//  }

  uint32_t Temp_Number = Number;
  uint8_t Expected_EndPosition = Position + NoOfLEDDigits - 1;


  while(Temp_Number){
      MAX7219_SendCommand(SPIx, Position, Temp_Number % 10);
      Temp_Number /= 10;
      Position ++;
  }

  while(Position <= Expected_EndPosition){
      MAX7219_SendCommand(SPIx, Position, 0);
      Position++;
  }
}

/**
  * @brief  Displays a left turn signal animation on the MAX7219 display.
  * @param  SPIx: Pointer to the SPI peripheral configuration structure.
  * @retval None
  */
void MAX7219_LeftSignal(SPI_RegDef_t* SPIx){
  for(uint8_t digit = MAX7219_REG_DIGIT0; digit <= MAX7219_REG_DIGIT3; digit++){
      if(digit == MAX7219_REG_DIGIT0){
	  MAX7219_SetDigitValue(SPIx, digit, 0x6);
      }
      else{
      MAX7219_SetDigitValue(SPIx, digit, 0x1);
      }
      Delay_ms(50); }
  Delay_ms(200);

  for(uint8_t digit = MAX7219_REG_DIGIT0; digit <= MAX7219_REG_DIGIT3; digit++){
      MAX7219_SetDigitValue(SPIx, digit, 0x0);
  }
  Delay_ms(200);
}

/**
  * @brief  Displays a right turn signal animation on the MAX7219 display.
  * @param  SPIx: Pointer to the SPI peripheral configuration structure.
  * @retval None
  */
void MAX7219_RightSignal(SPI_RegDef_t* SPIx){
  for(uint8_t digit = MAX7219_REG_DIGIT4; digit <= MAX7219_REG_DIGIT7; digit++){
      if(digit == MAX7219_REG_DIGIT7){
	  MAX7219_SetDigitValue(SPIx, digit, 0x30);
      }
      else{
      MAX7219_SetDigitValue(SPIx, digit, 0x1);
      }
      Delay_ms(50);
  }
  Delay_ms(200);

  for(uint8_t digit = MAX7219_REG_DIGIT4; digit <= MAX7219_REG_DIGIT7; digit++){
      MAX7219_SetDigitValue(SPIx, digit, 0x0);
  }
  Delay_ms(200);
}

/**
  * @brief  Displays a stop signal animation by blinking inward from the outer digits.
  * @param  SPIx: Pointer to the SPI peripheral configuration structure.
  * @retval None
  */
void MAX7219_StopSignal(SPI_RegDef_t* SPIx){
  uint8_t right = MAX7219_REG_DIGIT3;
  uint8_t left = MAX7219_REG_DIGIT4;
  while (right > 0)
  {
    if(right == MAX7219_REG_DIGIT0 || left == MAX7219_REG_DIGIT7){
      MAX7219_SetDigitValue(SPIx, MAX7219_REG_DIGIT0, 0x6);
      MAX7219_SetDigitValue(SPIx, MAX7219_REG_DIGIT7, 0x30);
    }else {
      MAX7219_SetDigitValue(SPIx, right, 0x1);
      MAX7219_SetDigitValue(SPIx, left, 0x1);
    }
    Delay_ms(50);
    right--;
    left++;
  }


  Delay_ms(200);
  for(uint8_t digit = MAX7219_REG_DIGIT0; digit <= MAX7219_REG_DIGIT7; digit++){
      MAX7219_SetDigitValue(SPIx, digit, 0x0);
    }
    Delay_ms(200);
}


