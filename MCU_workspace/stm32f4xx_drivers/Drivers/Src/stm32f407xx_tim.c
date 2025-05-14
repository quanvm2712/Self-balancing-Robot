/*
 * stm32f407xx_tim.c
 *
 *  Created on: May 12, 2025
 *      Author: nhduong
 */

#include "stm32f407xx_tim.h"

/**
  * @brief  Initializes the TIMx peripheral according to the specified parameters in the TIMx_Init.
  * @param  pTIMx where x can be (1..14) to select the TIM peripheral .
  * @param  clockState Specifies whether to ENABLE or DISABLE of the clock for the given TIM peripheral.
  * @retval None
  */
void TIM_PeriClockControl(TIM_RegDef_t *pTIMx, uint8_t clockState)
{
    if (clockState == ENABLE)
    {
        TIM2_CLK_ENABLE();
    }
    else
    {
       TIM2_CLK_DISABLE();

    }
}

/**
  * @brief  Time Base configuration
  * @param  TIMx TIM peripheral
  * @param  Structure TIM Base configuration structure
  * @retval None
  */
void TIM_Base_SetConfig(TIM_RegDef_t *pTIMx, uint32_t Prescaler, uint32_t Period, uint32_t DutyCycle)
{
	uint32_t tmpcr1 = 0;

	tmpcr1 = pTIMx->CR1;

	tmpcr1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
	tmpcr1  |= 0; // upCounter mode

	// Config clock
	tmpcr1 &= ~TIM_CR1_CKD;
	tmpcr1 |= 0; // no clock division

	pTIMx->CR1 = tmpcr1;

	// set Prescaler
	pTIMx->PSC = Prescaler;

	// set Period
	pTIMx->ARR = Period;

	// set duty cycle
	pTIMx->CCR1 = DutyCycle;

	// update generation
	pTIMx->EGR = TIM_EGR_UG;

	// Configure  Capture/Compare 1 output Polarity.
	pTIMx->CCER &= ~(1 << 1); // Active as high
	pTIMx->CCER |= (1 << 0); // Capture/Compare 1 output enable.

	// Select PWM mode
	pTIMx->CCMR1 |= 6 << 4; // PWM mode 1

	// Enable counter
	pTIMx->CR1 |= (1 << 0);
}

void GPIO_InitPWM(GPIO_HandleTypeDef *GPIO_InitStruct)
{
	GPIO_InitStruct->pGPIOx = GPIOA;
	GPIO_InitStruct->Init.Pin = GPIO_PIN_0;
	GPIO_InitStruct->Init.Mode = GPIO_MODE_AF;
	GPIO_InitStruct->Init.OPType = GPIO_OPTYPE_PP;
	GPIO_InitStruct->Init.Pull = GPIO_NOPULL;
	GPIO_InitStruct->Init.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct->Init.Alternate = 1;
	GPIO_Init(GPIO_InitStruct);
}


/**
  * @brief  Initializes the TIM PWM Time Base according to the specified
  *         parameters in the TIM_HandleTypeDef and initializes the associated handle.
  * @param  htim TIM PWM handle
  * @retval None
  */
void TIM_PWM_Init()
{
	GPIO_HandleTypeDef GPIO_InitStruct;
	GPIO_InitPWM(&GPIO_InitStruct);
	 /* Enable clock for the PWM */
	TIM_PeriClockControl(TIM2, ENABLE);

	 /* Init the base time for the PWM */
	TIM_Base_SetConfig(TIM2, 15, 999, 500);
}
