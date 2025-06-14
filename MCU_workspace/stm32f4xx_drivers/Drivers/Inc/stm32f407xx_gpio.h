/*
 * stm32f407xx_gpio.h
 *
 *  Created on: Feb 12, 2025
 *      Author: nhduong
 */

#ifndef INC_STM32F407XX_GPIO_H_
#define INC_STM32F407XX_GPIO_H_

#include "stm32f407xx.h"

/**
  * @brief GPIO Init structure definition
  */
typedef struct{
	uint8_t Pin; 			/*!< Specifies the GPIO pins to be configured.
					  	  	  	 This parameter can be any value of @ref GPIO_pins define 	*/

	uint8_t Mode; 			/*!< Specifies the operating mode for the selected pins.
								 This parameter can be a value of @ref GPIO_mode_define		*/

	uint8_t Pull;			/*!< Specifies the Pull-up or Pull-down activation for the selected pins.
								 This parameter can be a value of @ref GPIO_pull_define		*/

	uint8_t Speed;			/*!< Specifies the speed for the selected pins.
								 This parameter can be a value of @ref GPIO_speed_define	*/

	uint8_t OPType;			/*!< Specfies the output type for the selected pins.
								 This parameter can be a value of @ref GPIO_OPType_define	*/

	uint8_t Alternate;		/*!< Peripheral to be connected to the selected pins.
								 This parameter can be a value of @ref GPIO_Alternate_selection	*/

}GPIO_InitTypeDef;

/**
  * @brief GPIO Handle structure definition
  */
typedef struct
{
	GPIO_RegDef_t 		*pGPIOx;	/*!< This holds the base address of the GPIO port to which the pin belongs	*/

	GPIO_InitTypeDef 	Init;		/*!< This holds the GPIO pin configuration settings		*/

}GPIO_HandleTypeDef;


/** @defgroup GPIO_pins_define GPIO pins define
  *
  */
#define GPIO_PIN_0                 0   /* Pin 0 selected    */
#define GPIO_PIN_1                 1   /* Pin 1 selected    */
#define GPIO_PIN_2                 2   /* Pin 2 selected    */
#define GPIO_PIN_3                 3   /* Pin 3 selected    */
#define GPIO_PIN_4                 4   /* Pin 4 selected    */
#define GPIO_PIN_5                 5   /* Pin 5 selected    */
#define GPIO_PIN_6                 6   /* Pin 6 selected    */
#define GPIO_PIN_7                 7   /* Pin 7 selected    */
#define GPIO_PIN_8                 8   /* Pin 8 selected    */
#define GPIO_PIN_9                 9   /* Pin 9 selected    */
#define GPIO_PIN_10                10  /* Pin 10 selected   */
#define GPIO_PIN_11                11  /* Pin 11 selected   */
#define GPIO_PIN_12                12  /* Pin 12 selected   */
#define GPIO_PIN_13                13  /* Pin 13 selected   */
#define GPIO_PIN_14                14  /* Pin 14 selected   */
#define GPIO_PIN_15                15  /* Pin 15 selected   */

/** @defgroup GPIO_mode_define GPIO mode define
  *
  */
#define GPIO_MODE_INPUT                 0       /*!< Input Mode    */
#define GPIO_MODE_OUTPUT                1       /*!< Output Mode   */
#define GPIO_MODE_AF                    2       /*!< Alternate Function Mode */
#define GPIO_MODE_ANALOG                3       /*!< Analog Mode  */
#define GPIO_MODE_IT_FALLING            4       /*!< External Interrupt Mode with Falling edge trigger detection         */
#define GPIO_MODE_IT_RISING             5       /*!< External Interrupt Mode with Rising edge trigger detection          */
#define GPIO_MODE_IT_RISING_FALLING     6       /*!< External Interrupt Mode with Rising/Falling edge trigger detection  */

/** @defgroup GPIO_OPType_define GPIO OPType define
  *
  */
#define  GPIO_OPTYPE_PP            0                                 /*!< Output Push Pull Mode                 */
#define  GPIO_OPTYPE_OD            1                                 /*!< Output Open Drain Mode                */

/** @defgroup GPIO_speed_define GPIO speed define
  *
  */
#define GPIO_SPEED_LOW			0
#define GPIO_SPEED_MEDIUM		1
#define GPIO_SPEED_FAST			2
#define GPOI_SPEED_HIGH			3

/** @defgroup GPIO_pull_define GPIO pull define
  *
  */
#define  	GPIO_NOPULL        0   /*!< No Pull-up or Pull-down activation  */
#define  	GPIO_PULLUP        1   /*!< Pull-up activation                  */
#define  	GPIO_PULLDOWN      2   /*!< Pull-down activation                */

/** @defgroup Alternative function lists
  *
  */
#define		AF0				0
#define		AF1				1
#define		AF2				2
#define		AF3				3
#define		AF4				4
#define		AF5				5
#define		AF6				6
#define		AF7				7
#define		AF8				8
#define		AF9				9
#define		AF10				10
#define		AF11				11
#define		AF12				12
#define		AF13				13
#define		AF14				14
#define		AF15				15


/* Peripheral Clock setup function *****************************/
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t clockState);

/* Initialization and de-initialization functions *****************************/
void GPIO_Init(GPIO_HandleTypeDef *hGPIO);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

/* IO operation functions *****************************************************/
uint8_t GPIO_ReadPin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_pin);
uint16_t GPIO_ReadPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WritePin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_pin, uint8_t pinState);
void GPIO_WritePort(GPIO_RegDef_t *pGPIOx, uint16_t GPIO_pin);
void GPIO_TogglePin(GPIO_RegDef_t* pGPIOx, uint8_t GPIO_pin);
void GPIO_Initialize(GPIO_RegDef_t *GPIOx, uint8_t GPIO_Pin, uint8_t GPIO_Mode);
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t state);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void GPIO_IRQHandler(uint8_t GPIO_pin);
void AFIO_SelectAlternateFunction(GPIO_RegDef_t* GPIOx, uint8_t GPIO_Pin, uint8_t AlternateFunction);























#endif /* INC_STM32F407XX_GPIO_H_ */
