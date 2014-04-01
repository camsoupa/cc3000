################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/mss_i2c/mss_i2c.c 

OBJS += \
./drivers/mss_i2c/mss_i2c.o 

C_DEPS += \
./drivers/mss_i2c/mss_i2c.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/mss_i2c/%.o: ../drivers/mss_i2c/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -DACTEL_STDIO_THRU_UART -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


