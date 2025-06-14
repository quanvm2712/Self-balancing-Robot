/*
 * stm32f407xx_i2c.h
 *
 *  Created on: Feb 12, 2025
 *      Author: nhduong
 */

#ifndef INC_STM32F407XX_I2C_H_
#define INC_STM32F407XX_I2C_H_

#include "stm32f407xx.h"

/**
  * @brief  I2C Configuration Structure definition
  */
typedef struct
{
	uint32_t ClockSpeed;		/*!< Specifies the clock frequency.
								 	 This parameter can be value of @ref I2C_clock_speed			*/

	uint8_t DutyCycle;			/*!< Specifes the I2C fast mode duty cycle.
									 This parameter can be value of @ref I2C_duty_cycle_fast_mode 	*/

	uint8_t AckControl;			/*!< Specifies the acknowledgment control.
									 This parameter can be a value of @ref I2C_ack_control			*/

	uint8_t DeviceAddress;		/*!< Specifies the device own address.
									 This parameter can be a 7-bit or 10-bit address				*/

}I2C_InitTypeDef;

/**
  * @brief  SPI handle Structure definition
  */
typedef struct
{
    I2C_RegDef_t        *pI2Cx;         /*!< I2C registers base address         */

    I2C_InitTypeDef     Init;           /*!< I2C communication parameters       */

    uint8_t             *pTxBuffer;     /*!< Pointer to I2C Tx transfer Buffer  */

    uint8_t             *pRxBuffer;     /*!< Pointer to I2C Rx transfer Buffer  */

    uint32_t            TxLen;          /*!< I2C Tx Length                      */

    uint32_t            RxLen;          /*!< I2C Rx Length                      */

    uint8_t             TxRxState;          /*!< I2C communication State            */

    uint8_t             DevAddress;     /*!< I2C Target device address          */

    uint32_t            RxSize;         /*!< I2C Rx Size                        */

    uint8_t             Sr;             /*!< I2C repeated start value           */

}I2C_HandleTypeDef;

/** @defgroup I2C_State I2C State Definition
  *
  */
#define I2C_STATE_READY       				0
#define I2C_STATE_BUSY_TX 					1
#define I2C_STATE_BUSY_RX 					2


/** @defgroup I2C_Event I2C Event Definition
  *
  */
#define I2C_EV_TX_CMPLT  	 				0
#define I2C_EV_RX_CMPLT  	 				1
#define I2C_EV_STOP       					2
#define I2C_ERROR_BERR 	 					3
#define I2C_ERROR_ARLO  					4
#define I2C_ERROR_AF    					5
#define I2C_ERROR_OVR   					6
#define I2C_ERROR_TIMEOUT 					7
#define I2C_EV_DATA_REQ         			8
#define I2C_EV_DATA_RCV         			9

/** @defgroup I2C_duty_cycle_fast_mode I2C duty cycle fast mode
  *
  */
#define I2C_DUTYCYCLE_2                 	0
#define I2C_DUTYCYCLE_16_9              	1


/** @defgroup I2C_ack_control I2C ack control
  *
  */
#define I2C_ACK_DISABLE       				0
#define I2C_ACK_ENABLE        				1

/** @defgroup I2C_Repeated_Start I2C Repeated Start
  *
  */
#define I2C_SR_DISABLE  					0
#define I2C_SR_ENABLE  						1

/** @defgroup I2C_clock_speed I2C clock speed
  *
  */
#define I2C_CLOCKSPEED_SM                   100000
#define I2C_CLOCKSPEED_FM2K                 200000
#define I2C_CLOCK_PEED_FM4K                 400000

/** @defgroup I2C_Flags_definition I2C Flags Definition
  *
  */
#define I2C_FLAG_TXE   						( 1 << I2C_SR1_TXE) 	/* I2C status flag: Transmit data register empty */
#define I2C_FLAG_RXNE   					( 1 << I2C_SR1_RXNE)	/* I2C status flag: Receive data register not empty */
#define I2C_FLAG_SB							( 1 << I2C_SR1_SB) 		/* I2C status flag: Start condition generated (master mode) */
#define I2C_FLAG_OVR  						( 1 << I2C_SR1_OVR)		/* I2C status flag: Overrun/Underrun */
#define I2C_FLAG_AF   						( 1 << I2C_SR1_AF)		/* I2C status flag: Acknowledge failure */
#define I2C_FLAG_ARLO 						( 1 << I2C_SR1_ARLO)	/* I2C status flag: Arbitration lost (multi-master) */
#define I2C_FLAG_BERR 						( 1 << I2C_SR1_BERR)	/* I2C status flag: Bus error */
#define I2C_FLAG_STOPF 						( 1 << I2C_SR1_STOPF)	/* I2C status flag: Stop detection (slave mode) */
#define I2C_FLAG_ADD10 						( 1 << I2C_SR1_ADD10)	/* I2C status flag: 10-bit header sent (master mode) */
#define I2C_FLAG_BTF  						( 1 << I2C_SR1_BTF)		/* I2C status flag: Byte transfer finished */
#define I2C_FLAG_ADDR 						( 1 << I2C_SR1_ADDR)	/* I2C status flag: Address sent (master mode) or matched (slave mode) */
#define I2C_FLAG_TIMEOUT 					( 1 << I2C_SR1_TIMEOUT)	/* I2C status flag: Timeout or Tlow error */

/** @defgroup I2C_MemAddrSize I2C Memory address size
  *
  */
#define I2C_MEMADD_SIZE_8BIT  					1
#define I2C_MEMADD_SIZE_16BIT  					2

/**
  * @brief  I2C Status structures definition
  */
typedef enum
{
	I2C_OK       = 0x00U,
	I2C_ERROR    = 0x01U,
	I2C_BUSY     = 0x02U,
	I2C_TIMEOUT  = 0x03U
} I2C_StatusTypeDef;

/******************************************************************************************
 *                              APIs supported by this driver
 *       For more information about the APIs check the function definitions
 ******************************************************************************************/
/*
 * Peripheral Clock setup
 */
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t clockState);

/*
 * Init and De-init
 */
I2C_StatusTypeDef I2C_Init(I2C_HandleTypeDef *hi2c);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);
void I2C1_Init(I2C_HandleTypeDef *hi2c1);
void I2C1_GPIOInits(void);
void I2C2_Init(void);
void I2C2_GPIOInits(void);
/*
 * Data Transmit and Receive
 */
I2C_StatusTypeDef I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint8_t *pTxbuffer, uint32_t Len, uint8_t DevAddress, uint8_t Sr);
I2C_StatusTypeDef I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pRxBuffer, uint32_t Len, uint8_t DevAddress, uint8_t Sr);
I2C_StatusTypeDef I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
uint8_t I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pTxbuffer, uint32_t Len, uint8_t DevAddress, uint8_t Sr);
uint8_t I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pRxBuffer, uint8_t Len, uint8_t DevAddress, uint8_t Sr);

void I2C_CloseTransmitData(I2C_HandleTypeDef *hi2c);
void I2C_CloseReceiveData(I2C_HandleTypeDef *hi2c);


void I2C_Slave_Transmit(I2C_RegDef_t *pI2C,uint8_t data);
uint8_t I2C_Slave_Receive(I2C_RegDef_t *pI2C);

/*
 * IRQ Configuration and ISR handling
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t state);
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c);
void I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c);


/*
 * Other Peripheral Control APIs
 */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t controlState);
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx , uint32_t FlagName);
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t state);
void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx, uint8_t state);
void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx);
/*
 * Application callback
 */
void I2C_ApplicationEventCallback(I2C_HandleTypeDef *hi2c, uint8_t AppEv);










#endif /* INC_STM32F407XX_I2C_H_ */
