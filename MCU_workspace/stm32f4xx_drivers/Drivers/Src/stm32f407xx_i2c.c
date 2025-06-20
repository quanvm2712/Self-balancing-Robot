/*
 * stm32f407xx_i2c.c
 *
 *  Created on: Feb 23, 2025
 *      Author: nhduong
 */

#include "stm32f407xx_i2c.h"


static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_MasterRequestWrite(I2C_RegDef_t *pI2Cx, uint8_t DevAddress);
static void I2C_MasterRequestRead(I2C_RegDef_t *pI2Cx, uint8_t DevAddress);
static void I2C_Clear_ADDRFlag(I2C_HandleTypeDef *hi2c);

/* Private functions for I2C transfer IRQ handler */
static void I2C_MasterTransmit_TXE(I2C_HandleTypeDef *hi2c);
static void I2C_MasterTransmit_BTF(I2C_HandleTypeDef *hi2c);
static void I2C_MasterReceive_RXNE(I2C_HandleTypeDef *hi2c);
static void I2C_MasterReceive_BTF(I2C_HandleTypeDef *hi2c);


/**
  * @brief  Initialize I2C1 peripheral
  * @retval None
  */
 void I2C1_Init(I2C_HandleTypeDef *hi2c1)
{
  hi2c1->pI2Cx = I2C1;
  hi2c1->Init.ClockSpeed = I2C_CLOCKSPEED_SM;
  hi2c1->Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1->Init.DeviceAddress = MPU6050_ADDRESS;
  hi2c1->Init.AckControl = I2C_ACK_ENABLE;
  I2C_Init(hi2c1);
  I2C1_GPIOInits();
  I2C_PeripheralControl(I2C1,ENABLE);
}

 /**
   * @brief  Initialize GPIO pins for I2C1
   * @retval None
   *
   */
 void I2C1_GPIOInits(void)
 {
 	GPIO_HandleTypeDef GPIO_InitStruct;

 	GPIO_InitStruct.pGPIOx = GPIOB;
 	GPIO_InitStruct.Init.Pin = GPIO_PIN_6;  //SCL Pin
 	GPIO_InitStruct.Init.Mode = GPIO_MODE_AF;
 	GPIO_InitStruct.Init.Pull = GPIO_PULLUP;
 	GPIO_InitStruct.Init.OPType = GPIO_OPTYPE_OD;
 	GPIO_InitStruct.Init.Speed = GPIO_SPEED_FAST;
 	GPIO_InitStruct.Init.Alternate = GPIO_AF4_I2C1;
 	GPIO_Init(&GPIO_InitStruct);

 	GPIO_InitStruct.Init.Pin = GPIO_PIN_7; //SDA Pin
 	GPIO_Init(&GPIO_InitStruct);

 }

 /**
   * @brief  Initialize I2C2 peripheral
   * @retval None
   */
 void I2C2_Init(void)
 {
     I2C_HandleTypeDef hi2c2;
     hi2c2.pI2Cx = I2C2;
     hi2c2.Init.ClockSpeed = I2C_CLOCKSPEED_SM;
     hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
     hi2c2.Init.DeviceAddress = MPU6050_ADDRESS; // Adjust if targeting a different device
     hi2c2.Init.AckControl = I2C_ACK_ENABLE;
     I2C_Init(&hi2c2);
     I2C2_GPIOInits();
     I2C_PeripheralControl(I2C2,ENABLE);
 }

 /**
   * @brief  Initialize GPIO pins for I2C2
   * @retval None
   */
 void I2C2_GPIOInits(void)
 {
     GPIO_HandleTypeDef GPIO_InitStruct;

     GPIO_InitStruct.pGPIOx = GPIOB;
     GPIO_InitStruct.Init.Pin = GPIO_PIN_10;
     GPIO_InitStruct.Init.Mode = GPIO_MODE_AF;
     GPIO_InitStruct.Init.Pull = GPIO_PULLUP;
     GPIO_InitStruct.Init.Speed = GPIO_SPEED_FAST;
     GPIO_InitStruct.Init.Alternate = GPIO_AF4_I2C2;
     GPIO_Init(&GPIO_InitStruct);

     GPIO_InitStruct.Init.Pin = GPIO_PIN_11;
     GPIO_Init(&GPIO_InitStruct);
 }

/* --- Helper functions --- */
/**
  * @brief Generates a START condition on the I2C bus.
  * @param pI2Cx Pointer to I2C registers.
  * @retval None
  */
void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
{
    pI2Cx->CR1 |= (1 << I2C_CR1_START);
}

/**
  * @brief Generates a STOP condition on the I2C bus.
  * @param pI2Cx Pointer to I2C registers.
  * @retval None
  */
void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
    pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}

/**
  * @brief Transmits the slave address with the Write bit (0) for a master transmitter.
  * @param pI2Cx Pointer to I2C registers.
  * @param DevAddress The 7-bit slave address.
  * @retval None
  */
static void I2C_MasterRequestWrite(I2C_RegDef_t *pI2Cx, uint8_t DevAddress)
{
    DevAddress = DevAddress << 1;
    DevAddress &= ~(1); // Ensure the R/nW bit is 0 for write.
    pI2Cx->DR = DevAddress;
}

/**
  * @brief Transmits the slave address with the ReaD bit (1) for a master receiver.
  * @param pI2Cx Pointer to I2C registers.
  * @param DevAddress The 7-bit slave address.
  * @retval None
  */
static void I2C_MasterRequestRead(I2C_RegDef_t *pI2Cx, uint8_t DevAddress)
{
    DevAddress = DevAddress << 1;
    DevAddress |= 1; // Ensure the R/nW bit is 1 for read.
    pI2Cx->DR = DevAddress;
}

/**
  * @brief Clears the ADDR flag by reading SR1 and SR2.
  * @param hi2c Pointer to I2C handle.
  * @retval None
  */
static void I2C_Clear_ADDRFlag(I2C_HandleTypeDef *hi2c )
{
    uint32_t dummy_read;

    // Check if the device is in master mode by checking the MSL bit in SR2 (Master/Slave status)
    if (hi2c->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
    {
        // Device is in master mode
        if (hi2c->TxRxState == I2C_STATE_BUSY_TX)
        {
            // If we're in transmission mode and we're receiving only 1 byte
            if (hi2c->RxSize == 1)
            {
                // Disable Acknowledge (ACK) for the last byte
                I2C_ManageAcking(hi2c->pI2Cx, DISABLE);

                // Read SR1 and SR2 to clear the ADDR flag
                dummy_read = hi2c->pI2Cx->SR1;
                dummy_read = hi2c->pI2Cx->SR2;
                (void)dummy_read;  // Dummy read to clear the flags
            }
        }
        else
        {
            // Read SR1 and SR2 to clear the ADDR flag
            dummy_read = hi2c->pI2Cx->SR1;
            dummy_read = hi2c->pI2Cx->SR2;
            (void)dummy_read;  // Dummy read to clear the flags
        }
    }
    else
    {
        // Device is in slave mode
        // Read SR1 and SR2 to clear the ADDR flag in slave mode
        dummy_read = hi2c->pI2Cx->SR1;
        dummy_read = hi2c->pI2Cx->SR2;
        (void)dummy_read;  // Dummy read to clear the flags
    }
}

/**
  * @brief  Handle BTF flag for Master transmitter
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
static void I2C_MasterTransmit_BTF(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->pI2Cx->SR1 & (1 << I2C_SR1_TXE))  // Check if TXE flag is set
    {
        // BTF = 1, TXE = 1 (Byte Transfer Finished and Transmit Register Empty)
        if (hi2c->TxLen == 0)  // If there is no more data to send
        {
            // 1. Generate the STOP condition if the Repeated Start (Sr) flag is not set
            if (hi2c->Sr == I2C_SR_DISABLE)
            {
                I2C_GenerateStopCondition(hi2c->pI2Cx);
            }

            // 2. Reset the I2C handle data (clear buffers, reset state)
            I2C_CloseTransmitData(hi2c);

            // 3. Notify the application that transmission is complete
            I2C_ApplicationEventCallback(hi2c, I2C_EV_TX_CMPLT);
        }
    }
}


/**
  * @brief  Handle BTF flag for Master receiver
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
static void I2C_MasterReceive_BTF(I2C_HandleTypeDef *hi2c)
{
    /* Nothing to do */
}

/**
  * @brief  Handle STOPF flag for Slave
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
static void I2C_Slave_STOPF(I2C_HandleTypeDef *hi2c)
{
    // Clear the STOPF ( i.e 1) read SR1 2) Write to CR1 )
    hi2c->pI2Cx->CR1 |= 0x0000;

    // Notify the application that STOP is detected
    I2C_ApplicationEventCallback(hi2c,I2C_EV_STOP);
}

/**
  * @brief  Handle TXE flag for Master
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
static void I2C_MasterTransmit_TXE(I2C_HandleTypeDef *hi2c)
{
    if(hi2c->TxLen > 0)  // Check if there is more data to send
    {
        // 1. Load the data into the Data Register (DR)
        hi2c->pI2Cx->DR = *(hi2c->pTxBuffer);

        // 2. Decrement the length of data to be transmitted (TxLen)
        hi2c->TxLen--;

        // 3. Increment the buffer address to point to the next byte
        hi2c->pTxBuffer++;
    }
}


/**
  * @brief  Handle RXNE flag for Master
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
static void I2C_MasterReceive_RXNE(I2C_HandleTypeDef *hi2c)
{
    //We have to do the data reception
    if(hi2c->RxSize == 1)
    {
        *(hi2c->pRxBuffer) = hi2c->pI2Cx->DR;
        hi2c->RxLen--;

    }

    if(hi2c->RxSize > 1)
    {
        if(hi2c->RxLen == 2)
        {
            //clear the ack bit
            I2C_ManageAcking(hi2c->pI2Cx,DISABLE);
        }
            //read DR
            *(hi2c->pRxBuffer) = hi2c->pI2Cx->DR;
            hi2c->pRxBuffer++;
            hi2c->RxLen--;
    }

    if(hi2c->RxLen == 0 )
    {
        //close the I2C data reception and notify the application

        //1. generate the stop condition
        if(hi2c->Sr == I2C_SR_DISABLE)
            I2C_GenerateStopCondition(hi2c->pI2Cx);

        //2 . Close the I2C rx
        I2C_CloseReceiveData(hi2c);

        //3. Notify the application
        I2C_ApplicationEventCallback(hi2c,I2C_EV_RX_CMPLT);
    }
}

/**
  * @brief  Enables or disables the clock for the specified I2C peripheral.
  * @param  pI2Cx Pointer to I2C_RegDef_t structure representing I2C1, I2C2, or I2C3.
  * @param  clockState ENABLE (1) to enable the clock, DISABLE (0) to disable it.
  * @retval None
  */
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t clockState)
{
    if (clockState == ENABLE) {
        if (pI2Cx == I2C1) {
            I2C1_CLK_ENABLE();
        }else if (pI2Cx == I2C2) {
            I2C2_CLK_ENABLE();
        }else if (pI2Cx == I2C3) {
            I2C3_CLK_ENABLE();
        }
    }else {
        if (pI2Cx == I2C1) {
            I2C1_CLK_DISABLE();
        }else if (pI2Cx == I2C2) {
            I2C2_CLK_DISABLE();
        }else if (pI2Cx == I2C3) {
            I2C3_CLK_DISABLE();
        }
    }
}

/**
  * @brief  De-initializes the I2Cx peripheral registers to their default reset values.
  * @param  pI2Cx Pointer to I2C_RegDef_t structure representing I2C1, I2C2, or I2C3.
  * @retval None
  */
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
    if (pI2Cx == I2C1) {
        I2C1_REG_RESET();
    }else if (pI2Cx == I2C2) {
        I2C2_REG_RESET();
    }else if (pI2Cx == I2C3) {
        I2C3_REG_RESET();
    }
}

/**
  * @brief  Checks the status of a specific flag in the I2C Status Register (SR).
  * @param  pI2Cx Pointer to the I2C peripheral (I2C1, I2C2 and I2C3).
  * @param  FlagName The flag to check (e.g., I2C_FLAG_TXE, I2C_FLAG_RXNE, I2C_FLAG_BSY).
  * @retval FLAG_SET(1) or FLAG_RESET(0).
  */
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx , uint32_t FlagName)
{
    if(pI2Cx->SR1 & FlagName)
    {
        return FLAG_SET;
    }
    return FLAG_RESET;
}

/**
  * @brief  Initializes the I2C peripheral according to the specified parameters
  *         in the I2C_HandleTypeDef and initializes the associated handle.
  * @param  hi2c Pointer to an I2C_HandleTypeDef structure that contains
  *         the configuration information for the specified I2C peripheral.
  * @retval I2C status
  */
I2C_StatusTypeDef I2C_Init(I2C_HandleTypeDef *hi2c)
{
    uint32_t tempreg = 0;

    // Enable clock for I2Cx peripheral
    I2C_PeriClockControl(hi2c->pI2Cx, ENABLE);

    // Set the ACK control bit (Acknowledgment)
    tempreg |= hi2c->Init.AckControl << 10;
    hi2c->pI2Cx->CR1 = tempreg;

    // Configure the FREQ field of CR2 (Clock control register)
    tempreg = 0;
    tempreg = RCC_GetPCLK1_Value() / 1000000;  // Calculate the PCLK1 value in MHz
    hi2c->pI2Cx->CR2 = (tempreg & 0x3F);

    // Program the device's own address (7-bit address for I2C)
    tempreg = 0;
    tempreg |= hi2c->Init.DeviceAddress << 1;  // Set the 7-bit device address, shifting it to the left by 1 bit
    tempreg |= (1 << 14);
    hi2c->pI2Cx->OAR1 = tempreg;

    // CCR Calculation (Clock control register for I2C)
    uint32_t CCR_value = 0;
    tempreg = 0;

    if (hi2c->Init.ClockSpeed <= I2C_CLOCKSPEED_SM) {
        // Standard Mode (100 kHz)
        CCR_value = (RCC_GetPCLK1_Value() / (2 * hi2c->Init.ClockSpeed));
    } else {
        // Fast Mode (400 kHz)
        tempreg |= 1 << 15;
        tempreg |= (hi2c->Init.DutyCycle << 14);

        if (hi2c->Init.DutyCycle == I2C_DUTYCYCLE_2) {
            CCR_value = (RCC_GetPCLK1_Value() / (3 * hi2c->Init.ClockSpeed));  // Calculate CCR value for DutyCycle 2:1
        } else {
            CCR_value = (RCC_GetPCLK1_Value() / (25 * hi2c->Init.ClockSpeed));  // Calculate CCR value for DutyCycle 16:9
        }
    }

    tempreg |= (CCR_value & 0xFFF);
    hi2c->pI2Cx->CCR = tempreg;

    // TRISE Configuration (Tri-state time configuration)
    if (hi2c->Init.ClockSpeed <= I2C_CLOCKSPEED_SM) {
        // Standard Mode (100 kHz)
        tempreg = (RCC_GetPCLK1_Value() / 1000000U) + 1;
    } else {
        // Fast Mode (400 kHz)
        tempreg = ((RCC_GetPCLK1_Value() * 300) / 1000000000U) + 1;
    }
    hi2c->pI2Cx->TRISE = (tempreg & 0x3F);

    return I2C_OK;
}



/**
  * @brief  Transmits in master mode an amount of data in blocking mode.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  pTxbuffer Pointer to data buffer.
  * @param  Len Amount of data to be sent.
  * @param  DevAddress Target device address: The device's 7-bit address (shifted left before sending).
  * @param  Sr Start repeated
  * @retval I2C_status
  */
I2C_StatusTypeDef I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint8_t *pTxbuffer, uint32_t Len, uint8_t DevAddress, uint8_t Sr)
{
    // 1. Generate the START condition.
    I2C_GenerateStartCondition(hi2c->pI2Cx);

    // 2. Wait until the SB (Start Bit) flag is set.
    //    Until SB is cleared, SCL remains low (clock stretching).
    while (!(I2C_GetFlagStatus(hi2c->pI2Cx, I2C_FLAG_SB)));

    // 3. Send the slave address with the R/nW bit set to Write (0).
    I2C_MasterRequestWrite(hi2c->pI2Cx, DevAddress);

    // 4. Wait until the ADDR flag is set indicating address phase completion.
    while (!(I2C_GetFlagStatus(hi2c->pI2Cx, I2C_FLAG_ADDR)));

    // 5. Clear the ADDR flag (by reading SR1 then SR2).
    //    Until ADDR is cleared, SCL remains low.
    I2C_Clear_ADDRFlag(hi2c);

    // 6. Transmit data bytes until the length becomes 0.
    while (Len > 0)
    {
        // Wait until TXE (Transmit Data Register Empty) flag is set.
        while (!(I2C_GetFlagStatus(hi2c->pI2Cx, I2C_FLAG_TXE)));
        // Write data to the data register.
        hi2c->pI2Cx->DR = *pTxbuffer;
        pTxbuffer++;
        Len--;
    }

    // 7. Wait for the final byte transfer to complete:
    //    Wait until TXE and BTF (Byte Transfer Finished) flags are set.
    while (!(I2C_GetFlagStatus(hi2c->pI2Cx, I2C_FLAG_TXE)));
    while (!(I2C_GetFlagStatus(hi2c->pI2Cx, I2C_FLAG_BTF)));

    // 8. Generate the STOP condition if repeated start is disabled.
    //    Generating STOP automatically clears the BTF flag.
    if (Sr == I2C_SR_DISABLE) {
        I2C_GenerateStopCondition(hi2c->pI2Cx);
    }
    return I2C_OK;
}

/**
  * @brief  Receives in master mode an amount of data in blocking mode.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  pRxbuffer Pointer to data buffer.
  * @param  Len Amount of data to be sent.
  * @param  DevAddress Target device address: The device's 7-bit address (shifted left before sending).
  * @param  Sr Start repeated
  * @retval i2c_status
  */
I2C_StatusTypeDef I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pRxbuffer, uint32_t Len, uint8_t DevAddress, uint8_t Sr)
{
    // 1. Generate the START condition
    I2C_GenerateStartCondition(hi2c->pI2Cx);

     // 2. Wait until the SB (Start Bit) flag is set.
     //    Until SB is cleared, SCL remains low (clock stretching).
    while (!(I2C_GetFlagStatus(hi2c->pI2Cx, I2C_FLAG_SB)));

    // 3. Send the slave address with the R/nW bit set to Read (1).
    I2C_MasterRequestRead(hi2c->pI2Cx, DevAddress);

    // 4. Wait until the ADDR flag is set indicating address phase completion.
    while (!(I2C_GetFlagStatus(hi2c->pI2Cx, I2C_FLAG_ADDR)));

    // Read only 1 byte from slave
    if (Len == 1)
    {
        // Disable Acknowledge
        I2C_ManageAcking(hi2c->pI2Cx, I2C_ACK_DISABLE);

        // Clear the ADDR flag
        I2C_Clear_ADDRFlag(hi2c);

        // wait until RXNE becomes 1
        while (! (I2C_GetFlagStatus(hi2c->pI2Cx, I2C_FLAG_RXNE)));

        // generate the STOP condition
        if (Sr == I2C_SR_DISABLE) {
            I2C_GenerateStopCondition(hi2c->pI2Cx);
        }

        // Read the single data byte from the data register into the buffer
        *pRxbuffer = hi2c->pI2Cx->DR;
    }

    // Read data from slave when Len > 1
    if (Len > 1)
    {
        // clear the ADDR flag
        I2C_Clear_ADDRFlag(hi2c);

        // Read data until all bytes are received.
        for (uint32_t i = Len; i > 0; i--)
        {
            // wait until RXNE becomes 1
            while (! (I2C_GetFlagStatus(hi2c->pI2Cx, I2C_FLAG_RXNE)));

            // Check if this is the second-to-last byte (for NACK and Stop/ReStart)
            if (i == 2)
            {
                // Disable Acknowledge
                I2C_ManageAcking(hi2c->pI2Cx, I2C_ACK_DISABLE);

                // generate the STOP condition
                if (Sr == I2C_SR_DISABLE) {
                        I2C_GenerateStopCondition(hi2c->pI2Cx);
                }
            }

            // read the data from data register into buffer
            *pRxbuffer = hi2c->pI2Cx->DR;

            // increment the buffer address
            pRxbuffer++;
        }
    }
    //re-enable ACKing
    if(hi2c->Init.AckControl == I2C_ACK_ENABLE)
    {
        I2C_ManageAcking(hi2c->pI2Cx,I2C_ACK_ENABLE);
    }
    return I2C_OK;
}

/**
  * @brief  Read an amount of data in blocking mode from a specific memory address
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  MemAddress Internal memory address
  * @param  MemAddSize Size of internal memory address
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @retval i2c_status
  */
I2C_StatusTypeDef I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
	// 1. Send memory address in write mode
	uint8_t mem_addr[2];

	if (MemAddSize == I2C_MEMADD_SIZE_8BIT)
	{
		// 1byte
		mem_addr[0] = (uint8_t)MemAddress;
		I2C_Master_Transmit(hi2c, mem_addr, 1, DevAddress, I2C_SR_ENABLE); // repeated start
	}else {
		// 2byte
		mem_addr[0] = (uint8_t)(MemAddress >> 8);
		mem_addr[1] = (uint8_t)MemAddress;
		I2C_Master_Transmit(hi2c, mem_addr, 2, DevAddress, I2C_SR_ENABLE); // repeated start
	}
	// Read data from slave
	I2C_Master_Receive(hi2c, pData, Size, DevAddress, I2C_SR_DISABLE);

	return I2C_OK;
}

/**
  * @brief  Transmit in master mode an amount of data in non-blocking mode with Interrupt
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address
  * @param  pTxbuffer Pointer to data buffer
  * @param  Len Amount of data to be sent
  * @retval State of the mode
  */
uint8_t I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pTxbuffer, uint32_t Len, uint8_t DevAddress, uint8_t Sr)
{
    uint8_t state = hi2c->TxRxState;

     // Ensure I2C is not busy transmitting or receiving data
    if ((state != I2C_STATE_BUSY_TX) && (state != I2C_STATE_BUSY_RX))
    {
        // Set up the parameters for the transmit operation
        hi2c->pTxBuffer = pTxbuffer;
        hi2c->TxLen = Len;
        hi2c->TxRxState = I2C_STATE_BUSY_TX;
        hi2c->DevAddress = DevAddress;
        hi2c->Sr = Sr;

        // Generate the START condition to initiate communication
        I2C_GenerateStartCondition(hi2c->pI2Cx);

        // Implement code to enable ITBUFEN Control bit
        hi2c->pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);

        // Implement code to enable ITEVTEN Control bit
        hi2c->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

        // Implement code to enable ITERREN Control bit
        hi2c->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
    }

    return state;
}




/**
  * @brief  Receive in master mode an amount of data in non-blocking mode with Interrupt
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address
  * @param  pRxBuffer Pointer to data buffer
  * @param  Len Amount of data to be sent
  * @retval State of the mode
  */
uint8_t I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pRxBuffer, uint8_t Len, uint8_t DevAddress, uint8_t Sr)
{
    uint8_t state = hi2c->TxRxState;

    // Ensure I2C is not busy transmitting or receiving data
    if ((state != I2C_STATE_BUSY_TX) && (state != I2C_STATE_BUSY_RX))
    {
        // Set up the parameters for the receive operation
        hi2c->pRxBuffer = pRxBuffer;
        hi2c->RxLen = Len;
        hi2c->RxSize = Len;
        hi2c->TxRxState = I2C_STATE_BUSY_RX;
        hi2c->DevAddress = DevAddress;
        hi2c->Sr = Sr;

        // Generate the START condition to initiate communication
        I2C_GenerateStartCondition(hi2c->pI2Cx);

        // Enable the interrupt for buffer events
        hi2c->pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);

        // Enable the interrupt for event-based events
        hi2c->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

        // Enable the interrupt for error-based events
        hi2c->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
    }

    return state;
}

/**
  * @brief  Checks the ack control in the I2C peripheral.
  * @param  pI2Cx Pointer to the I2C peripheral (I2C1, I2C2 and I2C3).
  * @param  state I2C_ACK_ENABLE (1) to enable the ack, I2C_ACK_DISABLE (0) to disable it.
  * @retval None
  */
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t state)
{
    if (state == I2C_ACK_ENABLE)
    {
        pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
    }else
    {
        pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);
    }
}

/**
  * @brief  Checks the peripheral control in the I2C peripheral.
  * @param  pI2Cx Pointer to the I2C peripheral (I2C1, I2C2 and I2C3).
  * @param  controlState ENABLE (1) to enable the Peripheral, DISABLE (0) to disable it.
  * @retval None
  */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t controlState)
{
    if (controlState == ENABLE)
    {
        pI2Cx->CR1 |= (1 << I2C_CR1_PE);
    }else {
        pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
    }
}

/**
  * @brief  Enables or disables the specified IRQ number.
  * @param  IRQNumber Specifies the IRQ number.
  * @param  state ENABLE or DISABLE the IRQ.
  * @retval None
  */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t state)
{

    if(state == ENABLE)
    {
        if(IRQNumber <= 31)
        {
            //program ISER0 register
            *NVIC_ISER0 |= ( 1 << IRQNumber );

        }else if(IRQNumber > 31 && IRQNumber < 64 ) //32 to 63
        {
            //program ISER1 register
            *NVIC_ISER1 |= ( 1 << (IRQNumber % 32) );
        }
        else if(IRQNumber >= 64 && IRQNumber < 96 )
        {
            //program ISER2 register //64 to 95
            *NVIC_ISER3 |= ( 1 << (IRQNumber % 64) );
        }
    }else
    {
        if(IRQNumber <= 31)
        {
            //program ICER0 register
            *NVIC_ICER0 |= ( 1 << IRQNumber );
        }else if(IRQNumber > 31 && IRQNumber < 64 )
        {
            //program ICER1 register
            *NVIC_ICER1 |= ( 1 << (IRQNumber % 32) );
        }
        else if(IRQNumber >= 6 && IRQNumber < 96 )
        {
            //program ICER2 register
            *NVIC_ICER3 |= ( 1 << (IRQNumber % 64) );
        }
    }
}

/**
  * @brief  Configures the priority of an IRQ.
  * @param  IRQNumber Specifies the IRQ number.
  * @param  IRQPriority Specifies the priority level (0-255, lower is higher priority).
  * @retval None
  */
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
    //1. first lets find out the ipr register
    uint8_t iprx = IRQNumber / 4;
    uint8_t iprx_section  = IRQNumber %4 ;

    uint8_t shift_amount = ( 8 * iprx_section) + ( 8 - NO_PR_BITS_IMPLEMENTED) ;

    *(  NVIC_PR_BASEADDR + iprx ) |=  ( IRQPriority << shift_amount );
}

/**
  * @brief  Closes the I2C transmit (send) data by disabling interrupts and resetting state.
  * @param  hi2c: Pointer to the I2C_HandleTypeDef structure containing the configuration and state.
  * @retval None
  * @note   This function disables the interrupt-related control bits for I2C data transmission:
  *         - ITBUFEN (interrupt buffer enable) for buffer-related interrupts.
  *         - ITEVTEN (interrupt event enable) for event-related interrupts.
  *         After closing the transmission, the transmit state (`TxRxState`) is set to `READY`.
  *         The transmission buffer and length are reset to `NULL` and `0`, respectively.
  *         This function is typically used when finishing a transmit operation or resetting the I2C for new operations.
  */
void I2C_CloseTransmitData(I2C_HandleTypeDef *hi2c)
{
    // Disable ITBUFEN control bit
    hi2c->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

    // Disable ITEVTEN control bit
    hi2c->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

    hi2c->TxRxState = I2C_STATE_READY;   // Set state to ready
    hi2c->pTxBuffer = NULL;              // Reset TxBuffer
    hi2c->TxLen = 0;                     // Reset TxLen
}

/**
  * @brief  Closes the I2C receive data by disabling interrupts and resetting state.
  * @param  hi2c: Pointer to the I2C_HandleTypeDef structure containing the configuration and state.
  * @retval None
  * @note   This function disables the interrupt-related control bits for I2C data reception:
  *         - ITBUFEN (interrupt buffer enable) for buffer-related interrupts.
  *         - ITEVTEN (interrupt event enable) for event-related interrupts.
  *         After closing the reception, the receive state (`TxRxState`) is set to `READY`.
  *         The reception buffer and lengths are reset to `NULL` and `0`, respectively.
  *         If ACK control is enabled, it re-enables the ACK bit to resume I2C acknowledgment.
  *         This function is typically used after receiving data is complete or when the I2C interface needs to be reset.
  */
void I2C_CloseReceiveData(I2C_HandleTypeDef *hi2c)
{
    // Disable ITBUFEN control bit
    hi2c->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

    // Disable ITEVTEN control bit
    hi2c->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

    hi2c->TxRxState = I2C_STATE_READY;   // Set state to ready
    hi2c->pRxBuffer = NULL;              // Reset RxBuffer
    hi2c->RxLen = 0;                     // Reset RxLen
    hi2c->RxSize = 0;                    // Reset RxSize

    if (hi2c->Init.AckControl == I2C_ACK_ENABLE)
    {
        I2C_ManageAcking(hi2c->pI2Cx, ENABLE);  // Re-enable ACK if it's enabled in the configuration
    }
}


/**
  * @brief  Handle I2C event interrupt request.
  * @param  hi2c pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C module.
  * @retval None
  */
void I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c)
{
    //Interrupt handling for both master and slave mode of a device
    uint32_t temp1, temp2, temp3;

    temp1 = hi2c->pI2Cx->CR2 & (1 << I2C_CR2_ITEVTEN);
    temp2 = hi2c->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN);

    temp3 = hi2c->pI2Cx->SR1 & (1 << I2C_SR1_SB);

    // 1. Handle interrupt generated by SB event
    // Note: SB flag is only acplicable in Master mode
    if (temp1 && temp3)
    {
        //In this block lets executed the address phase
        if (hi2c->TxRxState == I2C_STATE_BUSY_TX)
        {
            I2C_MasterRequestWrite(hi2c->pI2Cx, hi2c->DevAddress);
        }else if (hi2c->TxRxState == I2C_STATE_BUSY_RX)
        {
            I2C_MasterRequestRead(hi2c->pI2Cx, hi2c->DevAddress);
        }
    }

    temp3 = hi2c->pI2Cx->SR1 & (1 << I2C_SR1_ADDR);
    // 2. Handle For interrupt generated by ADDR event
    // Note: When master mode : Address is sent
    //       When Slave mode  : Address matched with own address
    if (temp1 && temp3)
    {
        // interrupt is generated because of ADDR event
        I2C_Clear_ADDRFlag(hi2c);
    }

    temp3  = hi2c->pI2Cx->SR1 & ( 1 << I2C_SR1_BTF);
    // 3. Handle For interrupt generated by BTF(Byte Transfer Finished) event
    if (temp1 && temp3)
    {
        // BTF flag is set
        if (hi2c->TxRxState == I2C_STATE_BUSY_TX)
        {
            I2C_MasterTransmit_BTF(hi2c);
        }
        else if (hi2c->TxRxState == I2C_STATE_BUSY_RX)
        {
            I2C_MasterReceive_BTF(hi2c);
        }
    }

    temp3  = hi2c->pI2Cx->SR1 & ( 1 << I2C_SR1_STOPF);
    // 4. Handle For interrupt generated by STOPF event
    // Note : Stop detection flag is applicable only slave mode . For master this flag will never be set
    // The below code block will not be executed by the master since STOPF will not set in master mode
    if(temp1 && temp3)
    {
        I2C_Slave_STOPF(hi2c);
    }

    temp3  = hi2c->pI2Cx->SR1 & ( 1 << I2C_SR1_TXE);
    // 5. Handle For interrupt generated by TXE event
    if (temp1 && temp2 && temp3)
    {
        // check for device
        if (hi2c->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
        {
            // TXE flag is set
            // We have to do the data transmission
            if (hi2c->TxRxState == I2C_STATE_BUSY_TX)
            {
                I2C_MasterTransmit_TXE(hi2c);
            }
        }else
        {
            // Slave mode
            // make sure that the slave is really in transmitter mode
            if(hi2c->pI2Cx->SR2 & ( 1 << I2C_SR2_TRA))
            {
                I2C_ApplicationEventCallback(hi2c,I2C_EV_DATA_REQ);
            }
        }
    }

    temp3  = hi2c->pI2Cx->SR1 & ( 1 << I2C_SR1_RXNE);
    // 6. Handle For interrupt generated by RXNE event
    if(temp1 && temp2 && temp3)
    {
        // Check device mode
        if(hi2c->pI2Cx->SR2 & ( 1 << I2C_SR2_MSL))
        {
            // The device is master
            // RXNE flag is set
            if(hi2c->TxRxState == I2C_STATE_BUSY_RX)
            {
                I2C_MasterReceive_RXNE(hi2c);
            }
        }else
        {
            // Slave mode
            // Make sure that the slave is really in receiver mode
            if(!(hi2c->pI2Cx->SR2 & ( 1 << I2C_SR2_TRA)))
            {
                I2C_ApplicationEventCallback(hi2c,I2C_EV_DATA_RCV);
            }
        }
    }
}

/**
  * @brief  Handle I2C error interrupt request.
  * @param  hi2c pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for the specified I2C module.
  * @retval None
  */
void I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c)
{

    uint32_t temp1,temp2;

    //Know the status of  ITERREN control bit in the CR2
    temp2 = (hi2c->pI2Cx->CR2) & ( 1 << I2C_CR2_ITERREN);


/***********************Check for Bus error************************************/
    temp1 = (hi2c->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR);
    if(temp1  && temp2 )
    {
        //This is Bus error

        //Implement the code to clear the buss error flag
        hi2c->pI2Cx->SR1 &= ~( 1 << I2C_SR1_BERR);

        //Implement the code to notify the application about the error
       I2C_ApplicationEventCallback(hi2c,I2C_ERROR_BERR);
    }

/***********************Check for arbitration lost error************************************/
    temp1 = (hi2c->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO );
    if(temp1  && temp2)
    {
        //This is arbitration lost error

        //Implement the code to clear the arbitration lost error flag
        hi2c->pI2Cx->SR1 &= ~( 1 << I2C_SR1_ARLO);

        //Implement the code to notify the application about the error
        I2C_ApplicationEventCallback(hi2c,I2C_ERROR_ARLO);

    }

/***********************Check for ACK failure  error************************************/

    temp1 = (hi2c->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
    if(temp1  && temp2)
    {
        //This is ACK failure error

        //Implement the code to clear the ACK failure error flag
        hi2c->pI2Cx->SR1 &= ~( 1 << I2C_SR1_AF);

        //Implement the code to notify the application about the error
        I2C_ApplicationEventCallback(hi2c,I2C_ERROR_AF);
    }

/***********************Check for Overrun/underrun error************************************/
    temp1 = (hi2c->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
    if(temp1  && temp2)
    {
        //This is Overrun/underrun

        //Implement the code to clear the Overrun/underrun error flag
        hi2c->pI2Cx->SR1 &= ~( 1 << I2C_SR1_OVR);

        //Implement the code to notify the application about the error
        I2C_ApplicationEventCallback(hi2c,I2C_ERROR_OVR);
    }

/***********************Check for Time out error************************************/
    temp1 = (hi2c->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT);
    if(temp1  && temp2)
    {
        //This is Time out error

        //Implement the code to clear the Time out error flag
        hi2c->pI2Cx->SR1 &= ~( 1 << I2C_SR1_TIMEOUT);

        //Implement the code to notify the application about the error
        I2C_ApplicationEventCallback(hi2c,I2C_ERROR_TIMEOUT);
    }
}

__weak void I2C_ApplicationEventCallback(I2C_HandleTypeDef *hi2c, uint8_t AppEv)
{
	//This is a weak implementation . the user application may override this function.
}


