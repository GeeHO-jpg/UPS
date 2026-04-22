################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/sensor/AC_sensor.c \
../Core/Src/sensor/DC_sensor.c \
../Core/Src/sensor/NTS250P_INVERTER.c \
../Core/Src/sensor/NTS250_INVERTER.c 

OBJS += \
./Core/Src/sensor/AC_sensor.o \
./Core/Src/sensor/DC_sensor.o \
./Core/Src/sensor/NTS250P_INVERTER.o \
./Core/Src/sensor/NTS250_INVERTER.o 

C_DEPS += \
./Core/Src/sensor/AC_sensor.d \
./Core/Src/sensor/DC_sensor.d \
./Core/Src/sensor/NTS250P_INVERTER.d \
./Core/Src/sensor/NTS250_INVERTER.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/sensor/%.o Core/Src/sensor/%.su Core/Src/sensor/%.cyclo: ../Core/Src/sensor/%.c Core/Src/sensor/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-sensor

clean-Core-2f-Src-2f-sensor:
	-$(RM) ./Core/Src/sensor/AC_sensor.cyclo ./Core/Src/sensor/AC_sensor.d ./Core/Src/sensor/AC_sensor.o ./Core/Src/sensor/AC_sensor.su ./Core/Src/sensor/DC_sensor.cyclo ./Core/Src/sensor/DC_sensor.d ./Core/Src/sensor/DC_sensor.o ./Core/Src/sensor/DC_sensor.su ./Core/Src/sensor/NTS250P_INVERTER.cyclo ./Core/Src/sensor/NTS250P_INVERTER.d ./Core/Src/sensor/NTS250P_INVERTER.o ./Core/Src/sensor/NTS250P_INVERTER.su ./Core/Src/sensor/NTS250_INVERTER.cyclo ./Core/Src/sensor/NTS250_INVERTER.d ./Core/Src/sensor/NTS250_INVERTER.o ./Core/Src/sensor/NTS250_INVERTER.su

.PHONY: clean-Core-2f-Src-2f-sensor

