/*
 * stm32f407xx_spi.c
 *
 *  Created on: Feb 19, 2025
 *      Author: nhduong
 */

#include "stm32f407xx_spi.h"

static void spi_txe_interrupt_handler(SPI_HandleTypeDef *hspi);
static void spi_rxne_interrupt_handler(SPI_HandleTypeDef *hspi);
static void spi_ovr_error_interrupt_handler(SPI_HandleTypeDef *hspi);

/**
 * @brief  Enables or disables the clock for the specified SPI peripheral.
 * @param  pSPIx Pointer to SPI_RegDef_t structure representing SPI1, SPI2, or SPI3.
 * @param  clockState ENABLE (1) to enable the clock, DISABLE (0) to disable it.
 * @retval None
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t clockState) {

	if (clockState == ENABLE) {
		if (pSPIx == SPI1) {
			SPI1_CLK_ENABLE();
		} else if (pSPIx == SPI2) {
			SPI2_CLK_ENABLE();
		} else if (pSPIx == SPI3) {
			SPI3_CLK_ENABLE();
		}
	} else {
		if (pSPIx == SPI1) {
			SPI1_CLK_DISABLE();
		} else if (pSPIx == SPI2) {
			SPI2_CLK_DISABLE();
		} else if (pSPIx == SPI3) {
			SPI3_CLK_DISABLE();
		}
	}
}

/**
 * @brief Configures the SPI baud rate.
 *
 * @param SPIx     Pointer to the SPI peripheral.
 * @param BaudRate Baud rate prescaler value. This parameter can be one of the following values:
 * @arg SPI_BAUDRATEPRESCALER_2     : Baud rate = fPCLK / 2
 * @arg SPI_BAUDRATEPRESCALER_4     : Baud rate = fPCLK / 4
 * @arg SPI_BAUDRATEPRESCALER_8     : Baud rate = fPCLK / 8
 * @arg SPI_BAUDRATEPRESCALER_16    : Baud rate = fPCLK / 16
 * @arg SPI_BAUDRATEPRESCALER_32    : Baud rate = fPCLK / 32
 * @arg SPI_BAUDRATEPRESCALER_64    : Baud rate = fPCLK / 64
 * @arg SPI_BAUDRATEPRESCALER_128   : Baud rate = fPCLK / 128
 * @arg SPI_BAUDRATEPRESCALER_256   : Baud rate = fPCLK / 256
 */
void SPI_ConfigureBaudRate(SPI_RegDef_t *SPIx, uint8_t BaudRate) {
	SPIx->CR1 &= ~(0b111 << 3); //Reset value of BR bits
	SPIx->CR1 |= (BaudRate) << 3; //Set baudrate
}


/**
 * @brief Sets SPI to Master or Slave mode.
 *
 * @param SPIx     Pointer to the SPI peripheral.
 * @param SPI_Mode Set to 1 for Master, 0 for Slave.
 */
void SPI_SelectMasterSlaveMode(SPI_RegDef_t *SPIx, _Bool SPI_Mode) {
	SPIx->CR1 &= ~(0b1 << 2); //Reset the MSTR bit
	SPIx->CR1 |= SPI_Mode << 2;
}

/**
 * @brief Configures the clock polarity and phase.
 *
 * @param SPIx           Pointer to the SPI peripheral.
 * @param ClockPolarity  Set to 1 for idle high, 0 for idle low.
 * @param ClockPhase     Set to 1 for second edge, 0 for first edge.
 */
void SPI_ConfigureClock(SPI_RegDef_t *SPIx, _Bool ClockPolarity,
		_Bool ClockPhase) {
	SPIx->CR1 &= ~(0b11 << 0);  //Reset CPHA and CPOL bits
	SPIx->CR1 |= ClockPolarity << 1;
	SPIx->CR1 |= ClockPhase << 0;
}

/**
 * @brief Configures the SPI frame length.
 *
 * @param SPIx              Pointer to the SPI peripheral.
 * @param DataFrame_Length  Set to 1 for 16-bit, 0 for 8-bit frame.
 */
void SPI_ConfigureFrameLength(SPI_RegDef_t *SPIx, _Bool DataFrame_Length) {
	SPIx->CR1 &= ~(0b1 << 11); //Reset the DFF bit
	SPIx->CR1 |= DataFrame_Length << 11;
}


/**
 * @brief Configures the frame format (MSB/LSB first).
 *
 * @param SPIx         Pointer to the SPI peripheral.
 * @param FrameFormat  Set to 1 for LSB first, 0 for MSB first.
 */
void SPI_ConfigureFrameFormat(SPI_RegDef_t *SPIx, _Bool FrameFormat) {
	SPIx->CR1 &= ~(0b1 << 7);
	SPIx->CR1 |= (FrameFormat << 7);
}

/**
 * @brief Configures the NSS pin based on mode and software management.
 *
 * @param SPIx              Pointer to the SPI peripheral.
 * @param SPI_Mode          Set to 1 for Master, 0 for Slave.
 * @param IsSoftwareEnabled Set to 1 to use software NSS management.
 */
void SPI_ConfigureNSS(SPI_RegDef_t *SPIx, _Bool SPI_Mode,
		_Bool IsSoftwareEnabled) {
	SPIx->CR1 &= ~(0b1 << 9); 	//Reset SSM bit
	SPIx->CR1 &= ~(0b1 << 8); 	//Reset SSI bit
	SPIx->CR2 &= ~(0b1 << 2);	//Rest SSOE bit

	if ((SPI_Mode == SPI_MODE_SLAVE) && (IsSoftwareEnabled == TRUE)) {
		// Enable software NSS management in SLAVE mode
		SPIx->CR1 |= (0b1 << 9);
		SPIx->CR1 |= (0b1 << 8);
	} else if ((SPI_Mode == SPI_MODE_MASTER) && (IsSoftwareEnabled == TRUE)) {
		// Enable SSOE only when software NSS is disabled
		 // SPIx->CR2 |= (0b1 << 2);
	    SPI_SSIConfig(SPIx, ENABLE);

	} else if ((SPI_Mode == SPI_MODE_MASTER) && (IsSoftwareEnabled == FALSE))
	{
	    SPI_SSOEConfig(SPIx, ENABLE);
	}
}


/**
 * @brief Enables or disables the SPI peripheral.
 *
 * @param SPIx       Pointer to the SPI peripheral.
 * @param IsEnabled  Set to 1 to enable, 0 to disable SPI.
 */
void SPI_Enable(SPI_RegDef_t *SPIx, _Bool IsEnabled) {
	if (IsEnabled) {
		SPIx->CR1 |= (1 << 6);  // Set SPE bit
	} else {
		SPIx->CR1 &= ~(1 << 6); // Clear SPE bit
		uint32_t lastTicks = getTick();
		//Wait for BSY flag to reset, or break when the flag is not set within 100ms
		while ((SPIx->SR & (1 << 7))
				&& (getTick() - lastTicks < SPI_BUSY_TIMEOUT))
			;
	}
}


/**
 * @brief Initializes SPI GPIO alternate function pins.
 *
 * @param SPIx Pointer to the SPI peripheral.
 */
void SPI_ConfigureIOPin(SPI_RegDef_t *SPIx) {
  if (SPIx = SPI1) {
	GPIO_Initialize(GPIOA, GPIO_PIN_4, GPIO_MODE_AF);
	GPIO_Initialize(GPIOA, GPIO_PIN_5, GPIO_MODE_AF);
	GPIO_Initialize(GPIOA, GPIO_PIN_6, GPIO_MODE_AF);
	GPIO_Initialize(GPIOA, GPIO_PIN_7, GPIO_MODE_AF);

	AFIO_SelectAlternateFunction(GPIOA, GPIO_PIN_4,AF5);
	AFIO_SelectAlternateFunction(GPIOA, GPIO_PIN_5,AF5);
	AFIO_SelectAlternateFunction(GPIOA, GPIO_PIN_6,AF5);
	AFIO_SelectAlternateFunction(GPIOA, GPIO_PIN_7,AF5);
  }
}

/**
 * @brief Configures the SPI data direction mode.
 *
 * @param SPIx          Pointer to the SPI peripheral.
 * @param SPI_Direction Direction configuration value (e.g., full/half duplex).
 */
void SPI_ConfigureDirection(SPI_RegDef_t* SPIx, uint8_t SPI_Direction){
  SPIx->CR1 &= ~(0b11 << 14);
  //SPIx->CR1 |= (0b11 << 14); // Enable BIDIMODE and BIDIOE
}

/**
 * @brief Initializes the SPI peripheral with common settings. The baudrate prescaler is PCLK/16 when using this function.
 * For the direction, currently it's full duplex
 *
 * @param SPIx                Pointer to the SPI peripheral.
 * @param SPI_Mode            Set to 1 for Master, 0 for Slave.
 * @param SPI_ClockPolarity   Clock polarity setting.
 * @param SPI_ClockPhase      Clock phase setting.
 * @param FrameFormat         Frame format (MSB/LSB first).
 * @param DataFrame_Length    Frame length (8-bit/16-bit).
 * @param NSS_SoftwareEnabled Set to 1 to use software NSS.
 */
void SPI_Initialize(SPI_RegDef_t *SPIx, uint8_t SPI_Mode,
		_Bool SPI_ClockPolarity, _Bool SPI_ClockPhase, _Bool FrameFormat,
		_Bool DataFrame_Length, _Bool NSS_SoftwareEnabled)
{
  //Enable clock for SPI
  SPI_PeriClockControl(SPIx, ENABLE);

  //Configure GPIO for SPI
  SPI_ConfigureIOPin(SPIx);

  //Disable SPI
  SPI_Enable(SPIx, FALSE);

  //Prescaler is 16 as default
  SPI_ConfigureBaudRate(SPIx, SPI_BAUDRATEPRESCALER_16);

  //Configure clock phase and polarity
  SPI_ConfigureClock(SPIx, SPI_ClockPolarity, SPI_ClockPhase);

  //Configure data frame length.
  SPI_ConfigureFrameLength(SPIx, DataFrame_Length);

  //Configure data frame mode
  SPI_ConfigureFrameFormat(SPIx, FrameFormat);

  //Configure NSS Pin
  SPI_ConfigureNSS(SPIx, SPI_Mode, FALSE);

  //Configure master slave mode
  SPI_SelectMasterSlaveMode(SPIx, SPI_Mode);

  //SPI_ConfigureDirection(SPIx, 0);
}


/**
 * @brief  De-initializes the SPIx peripheral registers to their default reset values.
 * @param  pSPIx Pointer to SPI_RegDef_t structure representing SPI1, SPI2, or SPI3.
 * @retval None
 */
void SPI_DeInit(SPI_RegDef_t *pSPIx) {
	if (pSPIx == SPI1) {
		SPI1_REG_RESET();
	} else if (pSPIx == SPI2) {
		SPI2_REG_RESET();
	} else if (pSPIx == SPI3) {
		SPI3_REG_RESET();
	}
}
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t state) {
	if (state == ENABLE) {
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	} else {
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t state) {
	if (state == ENABLE) {
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	} else {
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	}
}
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t state) {
	if (state == ENABLE) {
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	} else {
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	}

}
/**
 * @brief  Checks the status of a specific flag in the SPI Status Register (SR).
 * @param  pSPIx Pointer to the SPI peripheral (SPI1, SPI2 and SPI3).
 * @param  FlagName The flag to check (e.g., SPI_FLAG_TXE, SPI_FLAG_RXNE, SPI_FLAG_BSY).
 * @retval FLAG_SET(1) or FLAG_RESET(0).
 */
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName) {
	return ((pSPIx->SR & FlagName) != 0) ? FLAG_SET : FLAG_RESET;
}
/**
 * @brief  Transmit an amount of data in blocking mode.
 * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
 *               the configuration information for SPI module.
 * @param  pTxBuffer pointer to transmit data buffer
 * @param  Length amount of data to be sent
 * @retval None
 */
_Bool SPI_Transmit(SPI_RegDef_t *pSPIx, const uint8_t *pTxBuffer, uint32_t Len) {
    SPI_PeripheralControl(pSPIx, ENABLE);
    while (Len > 0) {
	uint32_t ticks = getTick();
	// wait until TXE is set
	while (!(pSPIx->SR & SPI_FLAG_TXE)){
	    if(getTick() - ticks >= 50){
		return ERROR;
	    }
	}

	// check the data size
	if (pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
		// 16 bit in CR1
		// load data into the DR
		pSPIx->DR = *((uint16_t*) pTxBuffer);
		Len -= 2;
		pTxBuffer += 2;
	} else {
		// 8 bit in DFF
		pSPIx->DR = *((const uint8_t*) pTxBuffer);
		Len--;
		pTxBuffer++;
	}
    }
    while (SPI_GetFlagStatus(pSPIx, SPI_FLAG_BSY));
    SPI_PeripheralControl(pSPIx, DISABLE);

    return OK;
}

void SPI_Transmit1(SPI_RegDef_t *pSPIx, const uint8_t *pTxBuffer, uint32_t Len) {
	SPI_PeripheralControl(pSPIx, ENABLE);
	while (Len > 0) {
		// wait until TXE is set
		while (SPI_GetFlagStatus(pSPIx, SPI_FLAG_TXE) == FLAG_RESET)
			;

		// check the data size
		if (pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
			// 16 bit in CR1
			// load data into the DR
			pSPIx->DR = *((const uint16_t*) pTxBuffer);
			Len -= 2;
			pTxBuffer += 2;
		} else {
			// 8 bit in DFF
			*((__vo uint8_t*) &pSPIx->DR) = *((const uint8_t*) pTxBuffer);
			Len--;
			pTxBuffer++;
		}
	}
	while (SPI_GetFlagStatus(pSPIx, SPI_FLAG_BSY))
		;
	SPI_PeripheralControl(pSPIx, DISABLE);
}
/**
 * @brief  Receive an amount of data in blocking mode.
 * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
 *               the configuration information for SPI module.
 * @param  pRxBuffer pointer to receive data buffer
 * @param  Length amount of data to be receive
 * @retval None
 */
void SPI_Receive(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len) {
	SPI_PeripheralControl(pSPIx, ENABLE);
	while (Len > 0) {
		// wait until RXNE is set
		while (SPI_GetFlagStatus(pSPIx, SPI_FLAG_RXNE))
			;

		// Check the data size
		if (pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
			// 16 bit in CR1
			// load data from DR to pRxbuffer
			*((uint16_t*) pRxBuffer) = pSPIx->DR;
			Len -= 2;
			pRxBuffer += 2;
		} else {
			// 8 bit
			*pRxBuffer = pSPIx->DR;
			Len--;
			pRxBuffer++;
		}
	}
	while (SPI_GetFlagStatus(pSPIx, SPI_FLAG_BSY))
		;
	SPI_PeripheralControl(pSPIx, DISABLE);
}
void SPI_Receive1(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len) {
	SPI_PeripheralControl(pSPIx, ENABLE);
	while (Len > 0) {
		// wait until RXNE is set
		while (SPI_GetFlagStatus(pSPIx, SPI_FLAG_RXNE))
			;

		// Check the data size
		if (pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
			// 16 bit in CR1
			// load data from DR to pRxbuffer
			*((uint16_t*) pRxBuffer) = pSPIx->DR;
			Len -= 2;
			pRxBuffer += 2;
		} else {
			// 8 bit
			(*(uint8_t*) pRxBuffer) = *(__vo uint8_t*) &pSPIx->DR;
			Len--;
			pRxBuffer++;
		}
	}
	while (SPI_GetFlagStatus(pSPIx, SPI_FLAG_BSY))
		;
	SPI_PeripheralControl(pSPIx, DISABLE);
}

void SPI_TransmitReceive(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer,
		uint8_t *pRxBuffer, uint32_t Len) {
	// Enable the SPI peripheral
	SPI_PeripheralControl(pSPIx, ENABLE);

	while (Len > 0) {
		// Wait until TXE is set (transmit buffer empty)
		//while (SPI_GetFlagStatus(pSPIx, SPI_FLAG_TXE) == FLAG_RESET)
			;
		while (!(pSPIx->SR & SPI_FLAG_TXE));
					;
		// Check DFF bit in CR1 to determine 8-bit or 16-bit mode
		if (pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
			// 16-bit mode
			pSPIx->DR = *((uint16_t*) pTxBuffer);

			// Wait until RXNE is set
			// while (SPI_GetFlagStatus(pSPIx, SPI_FLAG_RXNE) == FLAG_RESET);
			while (!(pSPIx->SR & SPI_FLAG_RXNE));
			*((uint16_t*) pRxBuffer) = pSPIx->DR;

			Len -= 2;
			pTxBuffer += 2;
			pRxBuffer += 2;
		} else {
			// 8-bit mode
			*((__vo uint8_t*) &pSPIx->DR) = *((const uint8_t*) pTxBuffer);
			//while (SPI_GetFlagStatus(pSPIx, SPI_FLAG_RXNE) == FLAG_RESET);
			while ((pSPIx->SR & SPI_FLAG_RXNE));
			pTxBuffer++;
			// Wait until RXNE is set

			(*(uint8_t*) pRxBuffer) = *(__vo uint8_t*) &pSPIx->DR;

			Len--;
			pRxBuffer++;
		}
	}

	// Wait until SPI is not busy
	while (SPI_GetFlagStatus(pSPIx, SPI_FLAG_BSY) == FLAG_SET)
		;

	// Disable the SPI peripheral
	SPI_PeripheralControl(pSPIx, DISABLE);
}

/**
 * @brief  Transmit an amount of data in non-blocking mode with Interrupt.
 * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
 *               the configuration information for SPI module.
 * @param  pTxBuffer pointer to data buffer
 * @param  Len amount of data to be sent
 * @retval State of the mode
 */
uint8_t SPI_Transmit_IT(SPI_HandleTypeDef *hspi, uint8_t *pTxBuffer,
		uint32_t Len) {
	uint8_t state = hspi->TxState;

	if (state != SPI_STATE_BUSY_TX) {
		// save the Tx buffer address and len information in some global variables
		hspi->pTxBuffer = pTxBuffer;
		hspi->TxLen = Len;

		// mark the SPI state as busy in transmission so that
		// no other code can take over same SPI peripheral until transmisson is over
		hspi->TxState = SPI_STATE_BUSY_TX;

		// enable the TXEIE control bit to get interrupt whenever TXE is set in SR
		hspi->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
	}
	return state;
}

/**
 * @brief  Receive an amount of data in non-blocking mode with Interrupt.
 * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
 *               the configuration information for SPI module.
 * @param  pRxBuffer pointer to data buffer
 * @param  Len amount of data to be sent
 * @retval State of the mode
 */
uint8_t SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pRxBuffer,
		uint32_t Len) {
	uint8_t state = hspi->RxState;

	if (state != SPI_STATE_BUSY_RX) {
		// save the Rx buffer address and len information in some global variables
		hspi->pRxBuffer = pRxBuffer;
		hspi->RxLen = Len;

		// mark the SPI state as busy in transmission so that
		// no other code can take over same SPI peripheral until receive is over
		hspi->RxState = SPI_STATE_BUSY_RX;

		// enable the RXNEIE control bit to get interrupt whenever RX is set in SR
		hspi->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
	}
	return state;
}

/**
 * @brief  Enables or disables the specified IRQ number.
 * @param  IRQNumber Specifies the IRQ number.
 * @param  state ENABLE or DISABLE the IRQ.
 * @retval None
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t state) {

	if (state == ENABLE) {
		if (IRQNumber <= 31) {
			//program ISER0 register
			*NVIC_ISER0 |= (1 << IRQNumber);

		} else if (IRQNumber > 31 && IRQNumber < 64) //32 to 63
				{
			//program ISER1 register
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));
		} else if (IRQNumber >= 64 && IRQNumber < 96) {
			//program ISER2 register //64 to 95
			*NVIC_ISER2 |= (1 << (IRQNumber % 64));
		}
	} else {
		if (IRQNumber <= 31) {
			//program ICER0 register
			*NVIC_ICER0 |= (1 << IRQNumber);
		} else if (IRQNumber > 31 && IRQNumber < 64) {
			//program ICER1 register
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));
		} else if (IRQNumber >= 64 && IRQNumber < 96) {
			//program ICER2 register
			*NVIC_ICER2 |= (1 << (IRQNumber % 64));
		}
	}
}

/**
 * @brief  Configures the priority of an IRQ.
 * @param  IRQNumber Specifies the IRQ number.
 * @param  IRQPriority Specifies the priority level (0-255, lower is higher priority).
 * @retval None
 */
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority) {
	// lets find out the IPR register
	uint8_t iprx = IRQNumber / 4;
	uint8_t ipr_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * ipr_section) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASEADDR + iprx) &= ~(0xFF << shift_amount);
	*(NVIC_PR_BASEADDR + iprx) |= (IRQPriority << shift_amount);
}

/**
 * @brief  Handle SPI interrupt request.
 * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
 *               the configuration information for the specified SPI module.
 * @retval None
 */
void SPI_IRQHandler(SPI_HandleTypeDef *hspi) {
	uint8_t tmp1, tmp2;

	// check for TXE
	tmp1 = hspi->pSPIx->SR & (1 << SPI_SR_TXE);
	tmp2 = hspi->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);
	if (tmp1 && tmp2) {
		// handle TXE
		spi_txe_interrupt_handler(hspi);
	}

	// check for RXNE
	tmp1 = hspi->pSPIx->SR & (1 << SPI_SR_RXNE);
	tmp2 = hspi->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);
	if (tmp1 && tmp2) {
		// handle RXNE
		spi_rxne_interrupt_handler(hspi);
	}

	// check for ovr flag
	tmp1 = hspi->pSPIx->SR & (1 << SPI_SR_OVR);
	tmp2 = hspi->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);
	if (tmp1 && tmp2) {
		// handle OVR error
		spi_ovr_error_interrupt_handler(hspi);
	}
}

// some help function implementations

/**
 * @brief  Handle SPI TXE (Transmit Buffer Empty) interrupt.
 * @param  hspi Pointer to SPI_HandleTypeDef structure.
 * @retval None
 */
static void spi_txe_interrupt_handler(SPI_HandleTypeDef *hspi) {
	// check DFF bit in CR1
	if (hspi->pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
		// 16 bit
		hspi->pSPIx->DR = *((uint16_t*) hspi->pTxBuffer);
		hspi->TxLen -= 2;
		hspi->pTxBuffer += 2;
	} else {
		// 8 bit
		hspi->pSPIx->DR = *(hspi->pTxBuffer);
		hspi->TxLen--;
		hspi->pTxBuffer++;
	}
	if (!hspi->TxLen) {
		//Transmission is complete
		SPI_CloseTransmisson(hspi);

		// Call application event callback
		SPI_ApplicationEventCallback(hspi, SPI_EVENT_TX_COMPLETE);
	}
}

static void spi_rxne_interrupt_handler(SPI_HandleTypeDef *hspi) {
	// check DFF bit in CR1
	if (hspi->pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
		// 16 bit
		*((uint16_t*) hspi->pRxBuffer) = hspi->pSPIx->DR;
		hspi->RxLen -= 2;
		hspi->pRxBuffer += 2;
	} else {
		// 8 bit
		*(hspi->pRxBuffer) = hspi->pSPIx->DR;
		hspi->RxLen--;
		hspi->pRxBuffer++;
	}
	if (!hspi->RxLen) {
		//reception is complete
		SPI_CloseReception(hspi);

		// Call application event callback
		SPI_ApplicationEventCallback(hspi, SPI_EVENT_RX_COMPLETE);
	}
}

static void spi_ovr_error_interrupt_handler(SPI_HandleTypeDef *hspi) {
	// clear the ovr flag
	if (hspi->TxState != SPI_STATE_BUSY_TX) {
		SPI_ClearOVRFlag(hspi->pSPIx);
	}

	// Call application event callback
	SPI_ApplicationEventCallback(hspi, SPI_EVENT_OVR_ERROR);
}

void SPI_CloseTransmisson(SPI_HandleTypeDef *hspi) {
	hspi->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
	hspi->pTxBuffer = NULL;
	hspi->TxLen = 0;
	hspi->TxState = SPI_STATE_READY;

}

void SPI_CloseReception(SPI_HandleTypeDef *hspi) {
	hspi->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	hspi->pRxBuffer = NULL;
	hspi->RxLen = 0;
	hspi->RxState = SPI_STATE_READY;

}

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx) {
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void) temp;
}

__weak void SPI_ApplicationEventCallback(SPI_HandleTypeDef *hspi,
		uint8_t appEvent) {
	//This is a weak implementation . the user application may override this function.
}
