################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/apply/ADC_APP.c \
../Core/Src/apply/app.c \
../Core/Src/apply/ring_buffer.c \
../Core/Src/apply/wrap_function.c 

OBJS += \
./Core/Src/apply/ADC_APP.o \
./Core/Src/apply/app.o \
./Core/Src/apply/ring_buffer.o \
./Core/Src/apply/wrap_function.o 

C_DEPS += \
./Core/Src/apply/ADC_APP.d \
./Core/Src/apply/app.d \
./Core/Src/apply/ring_buffer.d \
./Core/Src/apply/wrap_function.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/apply/%.o Core/Src/apply/%.su Core/Src/apply/%.cyclo: ../Core/Src/apply/%.c Core/Src/apply/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-apply

clean-Core-2f-Src-2f-apply:
	-$(RM) ./Core/Src/apply/ADC_APP.cyclo ./Core/Src/apply/ADC_APP.d ./Core/Src/apply/ADC_APP.o ./Core/Src/apply/ADC_APP.su ./Core/Src/apply/app.cyclo ./Core/Src/apply/app.d ./Core/Src/apply/app.o ./Core/Src/apply/app.su ./Core/Src/apply/ring_buffer.cyclo ./Core/Src/apply/ring_buffer.d ./Core/Src/apply/ring_buffer.o ./Core/Src/apply/ring_buffer.su ./Core/Src/apply/wrap_function.cyclo ./Core/Src/apply/wrap_function.d ./Core/Src/apply/wrap_function.o ./Core/Src/apply/wrap_function.su

.PHONY: clean-Core-2f-Src-2f-apply

