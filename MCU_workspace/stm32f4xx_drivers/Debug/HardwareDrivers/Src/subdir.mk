################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HardwareDrivers/Src/MPU6050.c 

OBJS += \
./HardwareDrivers/Src/MPU6050.o 

C_DEPS += \
./HardwareDrivers/Src/MPU6050.d 


# Each subdirectory must supply rules for building sources it contributes
HardwareDrivers/Src/%.o HardwareDrivers/Src/%.su HardwareDrivers/Src/%.cyclo: ../HardwareDrivers/Src/%.c HardwareDrivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I"D:/Project/Self-balancing_1/MCU_workspace/stm32f4xx_drivers/Drivers/Inc" -I"D:/Project/Self-balancing_1/MCU_workspace/stm32f4xx_drivers/HardwareDrivers/Inc" -include"D:/Project/Self-balancing_1/MCU_workspace/stm32f4xx_drivers/Drivers/Inc/stm32f407xx.h" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=soft -mthumb -o "$@"

clean: clean-HardwareDrivers-2f-Src

clean-HardwareDrivers-2f-Src:
	-$(RM) ./HardwareDrivers/Src/MPU6050.cyclo ./HardwareDrivers/Src/MPU6050.d ./HardwareDrivers/Src/MPU6050.o ./HardwareDrivers/Src/MPU6050.su

.PHONY: clean-HardwareDrivers-2f-Src

