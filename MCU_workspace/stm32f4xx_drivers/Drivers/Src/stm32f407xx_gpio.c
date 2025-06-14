/*
 * stm32f407xx_gpio.c
 *
 *  Created on: Feb 15, 2025
 *      Author: nhduong
 */
#include "stm32f407xx_gpio.h"

/**
  * @brief  Initializes the GPIOx peripheral according to the specified parameters in the GPIO_Init.
  * @param  GPIOx where x can be (A..I) to select the GPIO peripheral .
  * @param  clockState Specifies whether to ENABLE or DISABLE of the clock for the given GPIO peripheral.
  * @retval None
  */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t clockState)
{
    if (clockState == ENABLE) {
        if (pGPIOx == GPIOA) {
            GPIOA_CLK_ENABLE();
        }else if (pGPIOx == GPIOB) {
            GPIOB_CLK_ENABLE();
        }else if (pGPIOx == GPIOC) {
            GPIOC_CLK_ENABLE();
        }else if (pGPIOx == GPIOD) {
            GPIOD_CLK_ENABLE();
        }else if (pGPIOx == GPIOE) {
            GPIOE_CLK_ENABLE();
        }else if (pGPIOx == GPIOF) {
            GPIOF_CLK_ENABLE();
        }else if (pGPIOx == GPIOG) {
            GPIOG_CLK_ENABLE();
        }else if (pGPIOx == GPIOH) {
            GPIOH_CLK_ENABLE();
        }else if (pGPIOx == GPIOI) {
            GPIOI_CLK_ENABLE();
        }
    }else {
        if (pGPIOx == GPIOA) {
            GPIOA_CLK_DISABLE();
        }else if (pGPIOx == GPIOB) {
            GPIOB_CLK_DISABLE();
        }else if (pGPIOx == GPIOC) {
            GPIOC_CLK_DISABLE();
        }else if (pGPIOx == GPIOD) {
            GPIOD_CLK_DISABLE();
        }else if (pGPIOx == GPIOE) {
            GPIOE_CLK_DISABLE();
        }else if (pGPIOx == GPIOF) {
            GPIOF_CLK_DISABLE();
        }else if (pGPIOx == GPIOG) {
            GPIOG_CLK_DISABLE();
        }else if (pGPIOx == GPIOH) {
            GPIOH_CLK_DISABLE();
        }else if (pGPIOx == GPIOI) {
            GPIOI_CLK_DISABLE();
        }
    }
}

/**
 * @brief  Configures the mode of the specified GPIO pin.
 *         Supports input, output, alternative function, analog, and external interrupt modes.
 *
 * @param  GPIOx      Pointer to GPIO peripheral (e.g., GPIOA, GPIOB).
 * @param  GPIO_PIN   GPIO pin number (0–15).
 * @param  GPIO_Mode  Mode to be set:
 *                    - GPIO_MODE_INPUT
 *                    - GPIO_MODE_OUTPUT
 *                    - GPIO_MODE_AF
 *                    - GPIO_MODE_ANALOG
 *                    - GPIO_MODE_IT_FALLING
 *                    - GPIO_MODE_IT_RISING
 *                    - GPIO_MODE_IT_RISING_FALLING
 *
 * @retval None
 */
void GPIO_ConfigureMode(GPIO_RegDef_t* GPIOx, uint8_t GPIO_PIN, uint8_t GPIO_Mode){
    if (GPIO_Mode <= GPIO_MODE_ANALOG){ //When the selected mode is not GPIO External Interrupt
	    //Reset bits value of selected pin.
	    GPIOx->MODER &= ~(0b11 << (GPIO_PIN * 2));
	    //Set the selected mode for the pin
	    GPIOx->MODER |= GPIO_Mode << (GPIO_PIN * 2);
    }
    else{//When the selected mode is GPIO External Interrupt
	    /******CONFIGURE EXTI Register******/
	    //Select triggered edge
	    if(GPIO_Mode == GPIO_MODE_IT_FALLING){
		    EXTI->FTSR |= (1 << GPIO_PIN);
		    EXTI->RTSR &= ~(1 << GPIO_PIN);
	    }
	    else if(GPIO_Mode == GPIO_MODE_IT_RISING){
		    EXTI->RTSR |= (1 << GPIO_PIN);
		    EXTI->FTSR &= ~(1 << GPIO_PIN);
	    }
	    else{
		    EXTI->RTSR |= (1 << GPIO_PIN);
		    EXTI->FTSR |= (1 << GPIO_PIN);
	    }

	    /******CONFIGURE SYSCFG******/
	    SYSCFG_CLK_ENABLE();	//Enable SYSCFG clock
	    uint8_t EXTICR_Index = GPIO_PIN / 4; //Get Register index of EXTICR
	    uint8_t EXTICR_StartBit = GPIO_PIN % 4;
	    uint8_t GPIO_Port =  GPIO_BASEADDR_TO_CODE(GPIOx);
	    SYSCFG->EXTICR[EXTICR_Index] &= ~(0b1111 << (EXTICR_StartBit * 4));
	    SYSCFG->EXTICR[EXTICR_Index] |= GPIO_Port << (EXTICR_StartBit * 4);

	    //Enable External interrupt for selected line
	    EXTI->IMR |= (1 << GPIO_PIN);

	    EXTI->PR |= (1 << GPIO_PIN);  // Clear pending flag
    }

}

/**
 * @brief  Configures the speed of the specified GPIO pin.
 *
 * @param  GPIOx       Pointer to GPIO peripheral.
 * @param  GPIO_Pin    GPIO pin number (0–15).
 * @param  GPIO_Speed  Speed level:
 *                     - GPIO_SPEED_LOW
 *                     - GPIO_SPEED_MEDIUM
 *                     - GPIO_SPEED_FAST
 *
 * @retval None
 */
void GPIO_ConfigureSpeed(GPIO_RegDef_t* GPIOx, uint8_t GPIO_Pin, uint8_t GPIO_Speed){
    uint8_t startBit = GPIO_Pin * 2;
    //Clear the value of selected pin
    GPIOx->OSPEEDR &= ~(0b11 << startBit);
    //Set GPIO Speed for selected Pin
    GPIOx->OSPEEDR |= GPIO_Speed << startBit;
}

/**
 * @brief  Sets the output type for the specified GPIO pin.
 *
 * @param  GPIOx            Pointer to GPIO peripheral.
 * @param  GPIO_Pin         GPIO pin number (0–15).
 * @param  GPIO_OutputType  Output type:
 *                          - GPIO_OPTYPE_PP (Push-pull)
 *                          - GPIO_OPTYPE_OD (Open-drain)
 *
 * @retval None
 */
void GPIO_ConfigureOutputType(GPIO_RegDef_t* GPIOx, uint8_t GPIO_Pin, _Bool GPIO_OutputType){
    GPIOx->OTYPER &= ~(1 << GPIO_Pin);  //Clear value of selected pin
    GPIOx->OTYPER |= GPIO_OutputType << GPIO_Pin; //Set GPIO Output type
}


/**
 * @brief  Configures pull-up/pull-down resistors for the specified GPIO pin.
 *
 * @param  GPIOx     Pointer to GPIO peripheral.
 * @param  GPIO_Pin  GPIO pin number (0–15).
 * @param  PullMode  Pull configuration:
 *                   - GPIO_NOPULL
 *                   - GPIO_PULLUP
 *                   - GPIO_PULLDOWN
 *
 * @retval None
 */
void GPIO_ConfigurePullUpDown(GPIO_RegDef_t* GPIOx, uint8_t GPIO_Pin, uint8_t PullMode){
    GPIOx->PUPDR &= ~(0b11 << (GPIO_Pin*2)); //Clear value of selected pin
    GPIOx->PUPDR |= PullMode << (GPIO_Pin * 2); //Set GPIO pull mode.
}


/**
 * @brief  Initializes the specified GPIO pin with the given mode.
 *         Enables peripheral clock and configures mode, output type, and pull settings.
 *
 * @param  GPIOx     Pointer to GPIO peripheral.
 * @param  GPIO_Pin  GPIO pin number (0–15).
 * @param  GPIO_Mode Desired mode (input, output, interrupt, etc.).
 *
 * @retval None
 */
void GPIO_Initialize(GPIO_RegDef_t *GPIOx, uint8_t GPIO_Pin, uint8_t GPIO_Mode){
      GPIO_PeriClockControl(GPIOx, ENABLE);

      GPIO_ConfigureMode(GPIOx, GPIO_Pin, GPIO_Mode);

      GPIO_ConfigureSpeed(GPIOx, GPIO_Pin, GPIO_SPEED_FAST);  //Fast speed as default.

      if(GPIO_Mode == GPIO_MODE_OUTPUT){
	      GPIO_ConfigureOutputType(GPIOx, GPIO_Pin, GPIO_OPTYPE_PP);  //Output push-pull as default
      }
      else if(GPIO_Mode == GPIO_MODE_INPUT || GPIO_Mode == GPIO_MODE_IT_RISING){
	      GPIO_ConfigurePullUpDown(GPIOx, GPIO_Pin, GPIO_PULLDOWN);  //If GPIO mode is input, GPIO_PULLDOWN is used as default.
      }
      else if(GPIO_Mode == GPIO_MODE_IT_FALLING){
	      GPIO_ConfigurePullUpDown(GPIOx, GPIO_Pin, GPIO_PULLUP);
      }
}

void AFIO_SelectAlternateFunction(GPIO_RegDef_t* GPIOx, uint8_t GPIO_Pin, uint8_t AlternateFunction){
    uint8_t StartBit;
    GPIO_PeriClockControl(GPIOx, ENABLE);
    if(GPIO_Pin <= GPIO_PIN_7){
	    StartBit = GPIO_Pin * 4;
	    GPIOx->AFR[0] &= ~(0b1111 << StartBit);
	    GPIOx->AFR[0] |= AlternateFunction << StartBit;
    }
    else{
	    StartBit = (GPIO_Pin - 8) * 4;
	    GPIOx->AFR[1] &= ~(0b1111 << StartBit);
	    GPIOx->AFR[1] |= AlternateFunction << StartBit;
    }
}

/**
  * @brief  Initializes the GPIO peripheral according to the specified parameters
  * 		in the GPIO_InitTypeDef and initialize the associated handle.
  * @param  hGPIO pointer to a GPIO_InitTypeDef structure that contains
  *         the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void GPIO_Init(GPIO_HandleTypeDef *hGPIO)
{
    uint32_t temp;
    // Enable the clock peripheral
    GPIO_PeriClockControl(hGPIO->pGPIOx, ENABLE);

    // 1. Configure mode
    if (hGPIO->Init.Mode <= GPIO_MODE_ANALOG) {
	    // The non interrupt mode
	    temp = (hGPIO->Init.Mode << (2 * hGPIO->Init.Pin));
	    hGPIO->pGPIOx->MODER &= ~(0x3 << (2 * hGPIO->Init.Pin)); // clearing
	    hGPIO->pGPIOx->MODER |= temp;
    }else {
	    // Interrupt mode
	    if (hGPIO->Init.Mode == GPIO_MODE_IT_FALLING) {
		    // Configure the FTSR
		    EXTI->FTSR |= (1 << hGPIO->Init.Pin);
		    // Clear the corresponding the RTSR
		    EXTI->RTSR &= ~(1 << hGPIO->Init.Pin);
	    }else if (hGPIO->Init.Mode == GPIO_MODE_IT_RISING) {
		    EXTI->RTSR |= (1 << hGPIO->Init.Pin);
		    EXTI->FTSR &= ~(1 << hGPIO->Init.Pin);
	    }else if (hGPIO->Init.Mode == GPIO_MODE_IT_RISING_FALLING) {
		    EXTI->RTSR |= (1 << hGPIO->Init.Pin);
		    EXTI->FTSR |= (1 << hGPIO->Init.Pin);
	    }
	    // Configure the GPIO port selection in SYSCFG_EXTICR
	    uint8_t tmp1 = hGPIO->Init.Pin / 4;
	    uint8_t tmp2 = hGPIO->Init.Pin % 4;
	    uint8_t portCode = GPIO_BASEADDR_TO_CODE(hGPIO->pGPIOx);
	    SYSCFG_CLK_ENABLE();
	    SYSCFG->EXTICR[tmp1] &= ~(0xF << (tmp2 * 4)); // Clear bits first
	    SYSCFG->EXTICR[tmp1] |= portCode << (tmp2 * 4);

	    // Enable the EXTI Interrupt delivery using IMR
	    EXTI->IMR |= 1 << hGPIO->Init.Pin;
    }

    // 2. Configure speed
    temp = ((hGPIO->Init.Speed) << (2 * hGPIO->Init.Pin));
    hGPIO->pGPIOx->OSPEEDR &= ~(0x3 << (2 * hGPIO->Init.Pin)); // clearing
    hGPIO->pGPIOx->OSPEEDR |= temp;

    // 3. Configure pull settings
    temp = (hGPIO->Init.Pull) << (2 * hGPIO->Init.Pin);
    hGPIO->pGPIOx->PUPDR &= ~(0x3 << (2 * hGPIO->Init.Pin)); // clearing
    hGPIO->pGPIOx->PUPDR |= temp;

    // 4. Configure OPType
    temp = (hGPIO->Init.OPType) << hGPIO->Init.Pin;
    hGPIO->pGPIOx->OTYPER &= ~(0x1 << hGPIO->Init.Pin); // clearing
    hGPIO->pGPIOx->OTYPER |= temp;

    // 5. Configure the alternate function
    if (hGPIO->Init.Mode == GPIO_MODE_AF) {
	    uint8_t temp1, temp2;

	    temp1 = hGPIO->Init.Pin / 8; // AFR[0] or AFR[1]
	    temp2 = hGPIO->Init.Pin % 8; // Position in AFR register
	    hGPIO->pGPIOx->AFR[temp1] &= ~(0xF << (4 * temp2)); // clearing
	    hGPIO->pGPIOx->AFR[temp1] |= (hGPIO->Init.Alternate << (4 * temp2));
    }
}
/**
  * @brief  De-initializes the GPIOx peripheral registers to their default reset values.
  * @param  GPIOx where x can be (A..I) to select the GPIO peripheral.
  * @retval None
  */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
    if(pGPIOx == GPIOA)
    {
        GPIOA_REG_RESET();
    }else if (pGPIOx == GPIOB)
    {
        GPIOB_REG_RESET();
    }else if (pGPIOx == GPIOC)
    {
        GPIOC_REG_RESET();
    }else if (pGPIOx == GPIOD)
    {
        GPIOD_REG_RESET();
    }else if (pGPIOx == GPIOE)
    {
        GPIOE_REG_RESET();
    }else if (pGPIOx == GPIOF)
    {
        GPIOF_REG_RESET();
    }else if (pGPIOx == GPIOG)
    {
        GPIOG_REG_RESET();
    }else if (pGPIOx == GPIOH)
    {
        GPIOH_REG_RESET();
    }else if (pGPIOx == GPIOI)
    {
        GPIOI_REG_RESET();
    }
}
/**
  * @brief  Reads the specified input pin.
  * @param  pGPIOx where x can be (A..I) to select the GPIO peripheral.
  * @param  GPIO_pin specifies the bit to read.
  *         This parameter can be GPIO_PIN_x where x can be (0..15).
  * @retval Pin state (0 = LOW, 1 = HIGH).
  */
uint8_t GPIO_ReadPin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_pin)
{
	uint8_t bitStatus;

	bitStatus = (uint8_t)((pGPIOx->IDR >> GPIO_pin) & 0x00000001);

	return bitStatus;
}
/**
  * @brief  Reads the entire input register of the specified GPIO port.
  * @param  pGPIOx where x can be (A..I) to select the GPIO peripheral).
  * @retval The 16-bit input port value (IDR register).
  */
uint16_t GPIO_ReadPort(GPIO_RegDef_t *pGPIOx)
{
	uint16_t value;

	value = (uint16_t)pGPIOx->IDR;

	return value;
}
/**
  * @brief  Sets or clears the selected GPIO pin.
  * @param  GPIOx where x can be (A..I) to select the GPIO peripheral.
  * @param  GPIO_Pin specifies the bit to be written.
  *          This parameter can be one of GPIO_PIN_x where x can be (0..15).
  * @param  pinState specifies the value to be written to the selected bit.
  *          This parameter can be one of the values:
  *            @arg GPIO_PIN_RESET: to clear the pin
  *            @arg GPIO_PIN_SET: to set the pin
  * @retval None
  */
void GPIO_WritePin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_pin, uint8_t pinState)
{
	if (pinState == GPIO_PIN_SET) {
		//write 1 to the output data register at the bit field corresponding to the pin number
		pGPIOx->ODR = (1 << GPIO_pin);
	}else {
		// write 0
		pGPIOx->ODR &= ~(1 << GPIO_pin);
	}
}
/**
  * @brief  Writes a 16-bit value to the entire GPIO port.
  * @param  GPIOx where x can be (A..I) to select the GPIO peripheral.
  * @param  GPIO_pin specifies the bit to be written.
  *          This parameter can be one of GPIO_PIN_x where x can be (0..15).
  * @retval None
  */
void GPIO_WritePort(GPIO_RegDef_t *pGPIOx, uint16_t GPIO_pin)
{
	pGPIOx->ODR = GPIO_pin;
}

/**
  * @brief  Toggles the specified GPIO pins.
  * @param  GPIOx Where x can be (A..I) to select the GPIO peripheral.
  * @param  GPIO_pin Specifies the pins to be toggled.
  * @retval None
  */
void GPIO_TogglePin(GPIO_RegDef_t* pGPIOx, uint8_t GPIO_pin)
{
	pGPIOx->ODR ^= (1 << GPIO_pin);
}

/**
  * @brief  Enables or disables the specified IRQ number.
  * @param  IRQNumber Specifies the IRQ number.
  * @param  state ENABLE or DISABLE the IRQ.
  * @retval None
  */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t state)
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
            *NVIC_ISER2 |= ( 1 << (IRQNumber % 64) );
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
        else if(IRQNumber >= 64 && IRQNumber < 96 )
        {
            //program ICER2 register
            *NVIC_ICER2 |= ( 1 << (IRQNumber % 64) );
        }
    }
}

/**
  * @brief  Configures the priority of an IRQ.
  * @param  IRQNumber Specifies the IRQ number.
  * @param  IRQPriority Specifies the priority level (0-255, lower is higher priority).
  * @retval None
  */
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// lets find out the IPR register
	uint8_t iprx = IRQNumber / 4;
	uint8_t ipr_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * ipr_section) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASEADDR + iprx) &= ~(0xFF << shift_amount);
	*(NVIC_PR_BASEADDR  + iprx) |= (IRQPriority << shift_amount);
}

/**
  * @brief  Handler the interrupt for a specific GPIO pin.
  * @param  GPIO_pin Specifies the pin number that triggered the interrupt.
  * @retval None
  */
void GPIO_IRQHandler(uint8_t GPIO_pin)
{
	// Clear the EXTI PR resgiter corresponding to the pin number
	if (EXTI->PR & (1 << GPIO_pin)) {
		// clear
		EXTI->PR |= (1 << GPIO_pin);
	}
}












