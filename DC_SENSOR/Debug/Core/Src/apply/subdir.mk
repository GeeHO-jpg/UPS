################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/apply/apply.c 

OBJS += \
./Core/Src/apply/apply.o 

C_DEPS += \
./Core/Src/apply/apply.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/apply/%.o Core/Src/apply/%.su Core/Src/apply/%.cyclo: ../Core/Src/apply/%.c Core/Src/apply/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-apply

clean-Core-2f-Src-2f-apply:
	-$(RM) ./Core/Src/apply/apply.cyclo ./Core/Src/apply/apply.d ./Core/Src/apply/apply.o ./Core/Src/apply/apply.su

.PHONY: clean-Core-2f-Src-2f-apply

