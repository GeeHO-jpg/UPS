################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/comm/adc_comm.c \
../Core/Src/comm/i2c_comm.c \
../Core/Src/comm/uart_comm.c 

OBJS += \
./Core/Src/comm/adc_comm.o \
./Core/Src/comm/i2c_comm.o \
./Core/Src/comm/uart_comm.o 

C_DEPS += \
./Core/Src/comm/adc_comm.d \
./Core/Src/comm/i2c_comm.d \
./Core/Src/comm/uart_comm.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/comm/%.o Core/Src/comm/%.su Core/Src/comm/%.cyclo: ../Core/Src/comm/%.c Core/Src/comm/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-comm

clean-Core-2f-Src-2f-comm:
	-$(RM) ./Core/Src/comm/adc_comm.cyclo ./Core/Src/comm/adc_comm.d ./Core/Src/comm/adc_comm.o ./Core/Src/comm/adc_comm.su ./Core/Src/comm/i2c_comm.cyclo ./Core/Src/comm/i2c_comm.d ./Core/Src/comm/i2c_comm.o ./Core/Src/comm/i2c_comm.su ./Core/Src/comm/uart_comm.cyclo ./Core/Src/comm/uart_comm.d ./Core/Src/comm/uart_comm.o ./Core/Src/comm/uart_comm.su

.PHONY: clean-Core-2f-Src-2f-comm

