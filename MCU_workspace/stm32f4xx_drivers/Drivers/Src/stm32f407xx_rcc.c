/*
 * stm32f407xx_rcc.c
 *
 *  Created on: Feb 23, 2025
 *      Author: nhduong
 */

#include "stm32f407xx_rcc.h"

uint16_t AHP_Prescaler[8] = {2, 4, 8, 16, 64, 128, 256, 512};
uint8_t APB_Prescaler[4] = {2, 4, 8, 16};

#define HSI_FREQ_DEFAULT 16000000 // Default HSI frequency
#define HSE_FREQ_DEFAULT 8000000  // Default HSE frequency

/**
 * @brief  Calculates the frequency of PCLK1 (APB1 peripheral clock).
 * @retval uint32_t PCLK1 frequency in Hz, or 0 if an error occurs.
 * @note   Assumes HSI = 16 MHz, HSE = 8 MHz, or PLL output as system clock source.
 *         Uses RCC_CFGR to determine clock source, AHB, and APB1 prescalers.
 */
/**
 * @brief  Calculates the frequency of PCLK1 (APB1 peripheral clock).
 * @retval uint32_t PCLK1 frequency in Hz, or 0 if an error occurs.
 * @note   Assumes HSI = 16 MHz, HSE = 8 MHz, or PLL output as system clock source.
 *         Uses RCC_CFGR to determine clock source, AHB, and APB1 prescalers.
 */
uint32_t RCC_GetPCLK1_Value(void)
{
    uint32_t PCLK1_value, systemClock;
    uint8_t clockSrc, temp, AHBP_value, APB1P_value;

    // Extract the clock source bits (bits 2:3 of RCC->CFGR)
    clockSrc = ((RCC->CFGR >> 2) & 0x3);
    if (clockSrc == 0) {
        // Clock source is HSI (internal oscillator)
        systemClock = HSI_FREQ_DEFAULT;  // e.g., 16 MHz
    } else if (clockSrc == 1) {
        // Clock source is HSE (external oscillator)
        systemClock = HSE_FREQ_DEFAULT;  // e.g., 8 MHz
    } else {
        // Clock source is PLL, so get the PLL output frequency
        systemClock = RCC_GetPLLOutputClock();
    }

    // Get the AHB prescaler value from RCC->CFGR (bits 4:7)
    temp = ((RCC->CFGR >> 4) & 0xF);
    if (temp < 8) {
        // No division; AHB clock is equal to system clock
        AHBP_value = 1;
    } else {
        // Division factor is determined from the AHP_Prescaler array.
        AHBP_value = AHP_Prescaler[temp - 8];
    }

    // Get the APB1 prescaler value from RCC->CFGR (bits 10:12)
    temp = ((RCC->CFGR >> 10) & 0x7);
    if (temp < 4) {
        // No division; APB1 clock is equal to AHB clock
        APB1P_value = 1;
    } else {
        // Division factor is determined from the APB_Prescaler array.
        APB1P_value = APB_Prescaler[temp - 4];
    }

    // Compute PCLK1: First, divide the system clock by the AHB prescaler,
    // then divide that result by the APB1 prescaler.
    PCLK1_value = (systemClock / AHBP_value) / APB1P_value;

    return PCLK1_value;
}


/**
 * @brief  Calculates the frequency of PCLK2 (APB2 peripheral clock).
 * @retval uint32_t PCLK2 frequency in Hz, or 0 if an error occurs.
 * @note   Assumes HSI = 16 MHz, HSE = 8 MHz, or PLL output as system clock source.
 *         Uses RCC_CFGR to determine clock source, AHB, and APB2 prescalers.
 */
uint32_t RCC_GetPCLK2_Value(void)
{
    uint32_t PCLK2_value, systemClock;
    uint8_t clockSrc, temp, AHBP_value, APB2P_value;

    // Determine system clock source (bits 2:3 of RCC->CFGR)
    clockSrc = ((RCC->CFGR >> 2) & 0x3);
    if (clockSrc == 0) {
        systemClock = HSI_FREQ_DEFAULT;  // e.g., 16 MHz
    } else if (clockSrc == 1) {
        systemClock = HSE_FREQ_DEFAULT;  // e.g., 8 MHz
    } else {
        systemClock = RCC_GetPLLOutputClock();
    }

    // Determine AHB prescaler (bits 4:7 of RCC->CFGR)
    temp = ((RCC->CFGR >> 4) & 0xF);
    if (temp < 8) {
        AHBP_value = 1;
    } else {
        AHBP_value = AHP_Prescaler[temp - 8];
    }

    // Determine APB2 prescaler (bits 13:15 of RCC->CFGR)
    temp = ((RCC->CFGR >> 13) & 0x7);
    if (temp < 4) {
        APB2P_value = 1;
    } else {
        APB2P_value = APB_Prescaler[temp - 4];
    }

    // Calculate PCLK2 by dividing the system clock by the AHB prescaler, then by the APB2 prescaler.
    PCLK2_value = (systemClock / AHBP_value) / APB2P_value;

    return PCLK2_value;
}


uint32_t  RCC_GetPLLOutputClock()
{
	return 0;
}
