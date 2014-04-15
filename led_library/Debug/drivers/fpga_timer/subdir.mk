################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fpga_timer/fpga_timer.c 

OBJS += \
./drivers/fpga_timer/fpga_timer.o 

C_DEPS += \
./drivers/fpga_timer/fpga_timer.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/fpga_timer/%.o: ../drivers/fpga_timer/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


