################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/rgb_led_driver/rgb_led.c 

OBJS += \
./drivers/rgb_led_driver/rgb_led.o 

C_DEPS += \
./drivers/rgb_led_driver/rgb_led.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/rgb_led_driver/%.o: ../drivers/rgb_led_driver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


