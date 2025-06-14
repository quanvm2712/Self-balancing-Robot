/*
 * stm32f407xx_tim.h
 *
 *  Created on: May 12, 2025
 *      Author: nhduong
 */

#ifndef INC_STM32F407XX_TIM_H_
#define INC_STM32F407XX_TIM_H_

#include "stm32f407xx.h"


/*
 * TIM Counter Mode
 */
#define TIM_COUNTERMODE_UP                 0  /*!< Counter used as upcounter */
#define TIM_COUNTERMODE_DOWN               1  /*!< Counter used as downcounter */
#define TIM_COUNTERMODE_CENTERALIGNED1     2  /*!< Center-aligned mode 1 */
#define TIM_COUNTERMODE_CENTERALIGNED2     3  /*!< Center-aligned mode 2 */
#define TIM_COUNTERMODE_CENTERALIGNED3     4  /*!< Center-aligned mode 3 */

/*
 * TIM Output Compare and PWM Modes
 */
#define TIM_OCMODE_FROZEN        0x0
#define TIM_OCMODE_ACTIVE        0x1
#define TIM_OCMODE_INACTIVE      0x2
#define TIM_OCMODE_TOGGLE        0x3
#define TIM_OCMODE_FORCE_LOW     0x4
#define TIM_OCMODE_FORCE_HIGH    0x5
#define TIM_OCMODE_PWM1          0x6
#define TIM_OCMODE_PWM2          0x7


/*
 * TIM Encoder Mode
 */
#define TIM_ENCODERMODE_TI1                0  /*!< Encoder mode 1 */
#define TIM_ENCODERMODE_TI2                1  /*!< Encoder mode 2 */
#define TIM_ENCODERMODE_TI12               2  /*!< Encoder mode 3 */

/*
 * TIM Encoder Input Polarity
 */
#define TIM_ENCODERINPUTPOLARITY_RISING    0  /*!< Rising edge polarity */
#define TIM_ENCODERINPUTPOLARITY_FALLING   1  /*!< Falling edge polarity */

/*
 * TIM Channels
 */
#define TIM_CHANNEL_1                      0
#define TIM_CHANNEL_2                      1
#define TIM_CHANNEL_3                      2
#define TIM_CHANNEL_4                      3

// Bit positions for channels in CCER
#define TIM_CCER_CC1P_Pos     1U
#define TIM_CCER_CC2P_Pos     5U
#define TIM_CCER_CC3P_Pos     9U
#define TIM_CCER_CC4P_Pos     13U

#define TIM_CCER_CC1E_Pos     0U
#define TIM_CCER_CC2E_Pos     4U
#define TIM_CCER_CC3E_Pos     8U
#define TIM_CCER_CC4E_Pos     12U


/*
 * TIM Output Compare Polarity
 */
#define TIM_OC_POLARITY_HIGH  0x0U
#define TIM_OC_POLARITY_LOW   0x1U



#define TIM_CCMR1_OC1M_Pos     	4U
#define TIM_CCMR2_OC1M_Pos     	12U
#define TIM_CCMR3_OC1M_Pos     	4U
#define TIM_CCMR4_OC1M_Pos     	12U


#define TIM_SMCR_SMS_Pos      0U

// mode encoder
#define ENCODER_MODE_TI1      1
#define ENCODER_MODE_TI2      2
#define ENCODER_MODE_TI12     3

// Input Capture Selection
#define TIM_IC_SELECTION_DIRECTTI    0x01U  // CCx is mapped on TIx (Direct)
#define TIM_IC_SELECTION_INDIRECTTI  0x02U  // CCx is mapped on TIx^ (Indirect)
#define TIM_IC_SELECTION_TRC         0x03U  // CCx is mapped on TRC (Trigger)

#define TIM_CCMR1_CC1S_Pos  0U
#define TIM_CCMR1_CC2S_Pos  8U
#define TIM_CCMR2_CC3S_Pos  0U
#define TIM_CCMR2_CC4S_Pos  8U

#define TIM_CCMR_CCxS_MASK  0x3U  // Mask for 2-bit CCxS field


uint16_t TIM_GetCounter(void);
void TIM_Base_SetConfig(TIM_RegDef_t *pTIMx, uint32_t Prescaler, uint32_t Period, uint32_t DutyCycle);
void TIM_PeriClockControl(TIM_RegDef_t *pTIMx, uint8_t clockState);
void GPIO_Init_TIM(uint8_t channel);
void TIM_SetChannelPolarity(TIM_RegDef_t *TIMx, uint8_t channel, uint8_t polarity);
void TIM_ChannelOutputControl(TIM_RegDef_t *TIMx, uint8_t channel, uint8_t State);
void TIM_ConfigTimeBase(TIM_RegDef_t *TIMx, uint32_t Prescaler, uint32_t Period, uint32_t DutyCycle, uint8_t Channel);
void TIM_SetOCMode(TIM_RegDef_t *TIMx, uint8_t channel, uint8_t OCmode);
void GPIO_Init_Encoder(void);
void TIM_SetConfigEncoder(TIM_RegDef_t *pTIMx, uint8_t CounterMode, uint8_t polarity, uint32_t Prescaler, uint32_t Period, uint8_t EncoderMode);
void TIM_Encoder_Init(TIM_RegDef_t *TIMx);


/*
 * Funtions that allow users to init and configure PWM
 */
void TIM_PWM_Init(TIM_RegDef_t *TIMx,uint8_t channel);
void TIM_SetConfigPWM(TIM_RegDef_t *pTIMx,
                   uint8_t CounterMode,
                   uint8_t Channel,
                   uint8_t polarity,
                   uint32_t Prescaler,
                   uint32_t Period,
                   uint32_t DutyCycle,
                   uint8_t OCMode);
void TIM_SetDuty(uint32_t DutyCycle);



#endif /* INC_STM32F407XX_TIM_H_ */
