################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Packet_RCSA/CircularBuffer.c \
../Core/Src/Packet_RCSA/Commands.c \
../Core/Src/Packet_RCSA/IDNumber.c \
../Core/Src/Packet_RCSA/SerialComm.c \
../Core/Src/Packet_RCSA/UDPPacket.c \
../Core/Src/Packet_RCSA/UDPPacketHeader.c \
../Core/Src/Packet_RCSA/crc_32.c 

OBJS += \
./Core/Src/Packet_RCSA/CircularBuffer.o \
./Core/Src/Packet_RCSA/Commands.o \
./Core/Src/Packet_RCSA/IDNumber.o \
./Core/Src/Packet_RCSA/SerialComm.o \
./Core/Src/Packet_RCSA/UDPPacket.o \
./Core/Src/Packet_RCSA/UDPPacketHeader.o \
./Core/Src/Packet_RCSA/crc_32.o 

C_DEPS += \
./Core/Src/Packet_RCSA/CircularBuffer.d \
./Core/Src/Packet_RCSA/Commands.d \
./Core/Src/Packet_RCSA/IDNumber.d \
./Core/Src/Packet_RCSA/SerialComm.d \
./Core/Src/Packet_RCSA/UDPPacket.d \
./Core/Src/Packet_RCSA/UDPPacketHeader.d \
./Core/Src/Packet_RCSA/crc_32.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Packet_RCSA/%.o Core/Src/Packet_RCSA/%.su Core/Src/Packet_RCSA/%.cyclo: ../Core/Src/Packet_RCSA/%.c Core/Src/Packet_RCSA/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Packet_RCSA

clean-Core-2f-Src-2f-Packet_RCSA:
	-$(RM) ./Core/Src/Packet_RCSA/CircularBuffer.cyclo ./Core/Src/Packet_RCSA/CircularBuffer.d ./Core/Src/Packet_RCSA/CircularBuffer.o ./Core/Src/Packet_RCSA/CircularBuffer.su ./Core/Src/Packet_RCSA/Commands.cyclo ./Core/Src/Packet_RCSA/Commands.d ./Core/Src/Packet_RCSA/Commands.o ./Core/Src/Packet_RCSA/Commands.su ./Core/Src/Packet_RCSA/IDNumber.cyclo ./Core/Src/Packet_RCSA/IDNumber.d ./Core/Src/Packet_RCSA/IDNumber.o ./Core/Src/Packet_RCSA/IDNumber.su ./Core/Src/Packet_RCSA/SerialComm.cyclo ./Core/Src/Packet_RCSA/SerialComm.d ./Core/Src/Packet_RCSA/SerialComm.o ./Core/Src/Packet_RCSA/SerialComm.su ./Core/Src/Packet_RCSA/UDPPacket.cyclo ./Core/Src/Packet_RCSA/UDPPacket.d ./Core/Src/Packet_RCSA/UDPPacket.o ./Core/Src/Packet_RCSA/UDPPacket.su ./Core/Src/Packet_RCSA/UDPPacketHeader.cyclo ./Core/Src/Packet_RCSA/UDPPacketHeader.d ./Core/Src/Packet_RCSA/UDPPacketHeader.o ./Core/Src/Packet_RCSA/UDPPacketHeader.su ./Core/Src/Packet_RCSA/crc_32.cyclo ./Core/Src/Packet_RCSA/crc_32.d ./Core/Src/Packet_RCSA/crc_32.o ./Core/Src/Packet_RCSA/crc_32.su

.PHONY: clean-Core-2f-Src-2f-Packet_RCSA

