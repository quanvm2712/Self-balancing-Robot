################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Src/SysTick.c \
../Drivers/Src/SystemClock.c \
../Drivers/Src/aht20_sensor.c \
../Drivers/Src/delay.c \
../Drivers/Src/stm32f407xx_gpio.c \
../Drivers/Src/stm32f407xx_i2c.c \
../Drivers/Src/stm32f407xx_rcc.c \
../Drivers/Src/stm32f407xx_spi.c \
../Drivers/Src/stm32f407xx_tim.c \
../Drivers/Src/stm32f407xx_usart.c 

OBJS += \
./Drivers/Src/SysTick.o \
./Drivers/Src/SystemClock.o \
./Drivers/Src/aht20_sensor.o \
./Drivers/Src/delay.o \
./Drivers/Src/stm32f407xx_gpio.o \
./Drivers/Src/stm32f407xx_i2c.o \
./Drivers/Src/stm32f407xx_rcc.o \
./Drivers/Src/stm32f407xx_spi.o \
./Drivers/Src/stm32f407xx_tim.o \
./Drivers/Src/stm32f407xx_usart.o 

C_DEPS += \
./Drivers/Src/SysTick.d \
./Drivers/Src/SystemClock.d \
./Drivers/Src/aht20_sensor.d \
./Drivers/Src/delay.d \
./Drivers/Src/stm32f407xx_gpio.d \
./Drivers/Src/stm32f407xx_i2c.d \
./Drivers/Src/stm32f407xx_rcc.d \
./Drivers/Src/stm32f407xx_spi.d \
./Drivers/Src/stm32f407xx_tim.d \
./Drivers/Src/stm32f407xx_usart.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Src/%.o Drivers/Src/%.su Drivers/Src/%.cyclo: ../Drivers/Src/%.c Drivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I"D:/Project/Self_Balancing_Robot/MCU_workspace/stm32f4xx_drivers/Drivers/Inc" -include"D:/Project/Self_Balancing_Robot/MCU_workspace/stm32f4xx_drivers/Drivers/Inc/stm32f407xx.h" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Src

clean-Drivers-2f-Src:
	-$(RM) ./Drivers/Src/SysTick.cyclo ./Drivers/Src/SysTick.d ./Drivers/Src/SysTick.o ./Drivers/Src/SysTick.su ./Drivers/Src/SystemClock.cyclo ./Drivers/Src/SystemClock.d ./Drivers/Src/SystemClock.o ./Drivers/Src/SystemClock.su ./Drivers/Src/aht20_sensor.cyclo ./Drivers/Src/aht20_sensor.d ./Drivers/Src/aht20_sensor.o ./Drivers/Src/aht20_sensor.su ./Drivers/Src/delay.cyclo ./Drivers/Src/delay.d ./Drivers/Src/delay.o ./Drivers/Src/delay.su ./Drivers/Src/stm32f407xx_gpio.cyclo ./Drivers/Src/stm32f407xx_gpio.d ./Drivers/Src/stm32f407xx_gpio.o ./Drivers/Src/stm32f407xx_gpio.su ./Drivers/Src/stm32f407xx_i2c.cyclo ./Drivers/Src/stm32f407xx_i2c.d ./Drivers/Src/stm32f407xx_i2c.o ./Drivers/Src/stm32f407xx_i2c.su ./Drivers/Src/stm32f407xx_rcc.cyclo ./Drivers/Src/stm32f407xx_rcc.d ./Drivers/Src/stm32f407xx_rcc.o ./Drivers/Src/stm32f407xx_rcc.su ./Drivers/Src/stm32f407xx_spi.cyclo ./Drivers/Src/stm32f407xx_spi.d ./Drivers/Src/stm32f407xx_spi.o ./Drivers/Src/stm32f407xx_spi.su ./Drivers/Src/stm32f407xx_tim.cyclo ./Drivers/Src/stm32f407xx_tim.d ./Drivers/Src/stm32f407xx_tim.o ./Drivers/Src/stm32f407xx_tim.su ./Drivers/Src/stm32f407xx_usart.cyclo ./Drivers/Src/stm32f407xx_usart.d ./Drivers/Src/stm32f407xx_usart.o ./Drivers/Src/stm32f407xx_usart.su

.PHONY: clean-Drivers-2f-Src

