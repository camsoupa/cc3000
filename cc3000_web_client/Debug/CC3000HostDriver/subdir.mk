################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CC3000HostDriver/cc3000_common.c \
../CC3000HostDriver/evnt_handler.c \
../CC3000HostDriver/hci.c \
../CC3000HostDriver/matt_spi.c \
../CC3000HostDriver/netapp.c \
../CC3000HostDriver/nvmem.c \
../CC3000HostDriver/os.c \
../CC3000HostDriver/socket.c \
../CC3000HostDriver/wlan.c 

OBJS += \
./CC3000HostDriver/cc3000_common.o \
./CC3000HostDriver/evnt_handler.o \
./CC3000HostDriver/hci.o \
./CC3000HostDriver/matt_spi.o \
./CC3000HostDriver/netapp.o \
./CC3000HostDriver/nvmem.o \
./CC3000HostDriver/os.o \
./CC3000HostDriver/socket.o \
./CC3000HostDriver/wlan.o 

C_DEPS += \
./CC3000HostDriver/cc3000_common.d \
./CC3000HostDriver/evnt_handler.d \
./CC3000HostDriver/hci.d \
./CC3000HostDriver/matt_spi.d \
./CC3000HostDriver/netapp.d \
./CC3000HostDriver/nvmem.d \
./CC3000HostDriver/os.d \
./CC3000HostDriver/socket.d \
./CC3000HostDriver/wlan.d 


# Each subdirectory must supply rules for building sources it contributes
CC3000HostDriver/%.o: ../CC3000HostDriver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -DACTEL_STDIO_THRU_UART -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


