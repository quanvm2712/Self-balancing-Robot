/*
 * MAX7219.c
 *
 *  Created on: May 18, 2025
 *      Author: quanvm198
 */
#include "MAX7219.h"



void MAX7219_SendCommand(SPI_RegDef_t* SPIx, uint8_t Address, uint8_t Data){
  uint8_t TX_Data[2] = {Address, Data};

  SPI_Transmit(SPIx, TX_Data, 2);
}


void MAX7219_OperationMode(SPI_RegDef_t* SPIx, _Bool Mode){
  MAX7219_SendCommand(SPIx, MAX7219_REG_SHUTDOWN, Mode);
}

void MAX7219_SetDecodeMode(SPI_RegDef_t* SPIx, uint8_t DecodeMode){
  MAX7219_SendCommand(SPIx, MAX7219_REG_DECODE_MODE, DecodeMode);
}

void MAX7219_SetIntensity(SPI_RegDef_t* SPIx, uint8_t IntensityLevel){
  MAX7219_SendCommand(SPIx, MAX7219_REG_INTENSITY, IntensityLevel);
}

void MAX7219_SetScanLimit(SPI_RegDef_t* SPIx, uint8_t ScanLimit){
  MAX7219_SendCommand(SPIx, MAX7219_REG_SCAN_LIMIT, ScanLimit);
}

void MAX7219_Init(SPI_RegDef_t* SPIx, uint8_t DecodeMode, uint8_t IntensityLevel, uint8_t ScanLimit){
  MAX7219_Clean(SPIx);
  MAX7219_OperationMode(SPIx, MAX7219_NORMAL_OPERATION);
  MAX7219_SetDecodeMode(SPIx, DecodeMode);
  MAX7219_SetIntensity(SPIx, IntensityLevel);
  MAX7219_SetScanLimit(SPIx, ScanLimit);
}

void MAX7219_SetDigitValue(SPI_RegDef_t* SPIx, uint8_t Digit, uint8_t Value){
  MAX7219_SendCommand(SPIx, Digit, Value);
}

void MAX7219_TestLED(SPI_RegDef_t* SPIx, _Bool IsEnabled){
  MAX7219_SendCommand(SPIx, MAX7219_REG_DISPLAY_TEST, IsEnabled);
}

void MAX7219_Clean(SPI_RegDef_t* SPIx){
  for(int count = 0; count <= 0xC; count++){
      MAX7219_SendCommand(SPIx, count, 0);
  }
  MAX7219_SendCommand(SPIx, 0xF, 0);
}

uint32_t POWER_10(uint8_t x){
  double result = 1;
  while (x--) {
      result *= 10;
  }
  return result;
}

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


