################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HardwareDriver/Src/DCMotor.c \
../HardwareDriver/Src/MAX7219.c \
../HardwareDriver/Src/MPU6050.c \
../HardwareDriver/Src/SR05.c 

OBJS += \
./HardwareDriver/Src/DCMotor.o \
./HardwareDriver/Src/MAX7219.o \
./HardwareDriver/Src/MPU6050.o \
./HardwareDriver/Src/SR05.o 

C_DEPS += \
./HardwareDriver/Src/DCMotor.d \
./HardwareDriver/Src/MAX7219.d \
./HardwareDriver/Src/MPU6050.d \
./HardwareDriver/Src/SR05.d 


# Each subdirectory must supply rules for building sources it contributes
HardwareDriver/Src/%.o HardwareDriver/Src/%.su HardwareDriver/Src/%.cyclo: ../HardwareDriver/Src/%.c HardwareDriver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I"D:/Project/Self-balancing_1/MCU_workspace/stm32f4xx_drivers/Drivers/Inc" -I"D:/Project/Self-balancing_1/MCU_workspace/stm32f4xx_drivers/HardwareDriver/Inc" -include"D:/Project/Self-balancing_1/MCU_workspace/stm32f4xx_drivers/Drivers/Inc/stm32f407xx.h" -O0 -ffunction-sections -fdata-sections -Wall -Wno-comment -Wno-parentheses -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=soft -mthumb -o "$@"

clean: clean-HardwareDriver-2f-Src

clean-HardwareDriver-2f-Src:
	-$(RM) ./HardwareDriver/Src/DCMotor.cyclo ./HardwareDriver/Src/DCMotor.d ./HardwareDriver/Src/DCMotor.o ./HardwareDriver/Src/DCMotor.su ./HardwareDriver/Src/MAX7219.cyclo ./HardwareDriver/Src/MAX7219.d ./HardwareDriver/Src/MAX7219.o ./HardwareDriver/Src/MAX7219.su ./HardwareDriver/Src/MPU6050.cyclo ./HardwareDriver/Src/MPU6050.d ./HardwareDriver/Src/MPU6050.o ./HardwareDriver/Src/MPU6050.su ./HardwareDriver/Src/SR05.cyclo ./HardwareDriver/Src/SR05.d ./HardwareDriver/Src/SR05.o ./HardwareDriver/Src/SR05.su

.PHONY: clean-HardwareDriver-2f-Src

