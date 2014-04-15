################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include drivers/rgb_led_driver/subdir.mk
-include drivers/mss_gpio/subdir.mk
-include drivers/fpga_timer/subdir.mk
-include CMSIS/subdir.mk
-include CMSIS/startup_gcc/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
MEMORYMAP += \
memory-map.xml \

IHEXFILES += \
led_library.hex \

SRECFILES += \
led_library.srec \

LISTINGS += \
led_library.lst \


# All Target
all: led_library secondary-outputs

# Tool invocations
led_library: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU C Linker'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -specs=bare.specs -T../CMSIS/startup_gcc/debug-in-actel-smartfusion-esram.ld -Wl,-Map=$(EXECUTABLE).map -o"led_library" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

memory-map.xml: $(EXECUTABLE)
	@echo 'Invoking: Memory map generator'
	actel-map -o "memory-map.xml" $(EXECUTABLE) $(BUILDCMD)
	@echo 'Finished building: $@'
	@echo ' '

led_library.hex: $(EXECUTABLE)
	@echo 'Invoking: GNU Intel Hex File Generator'
	arm-none-eabi-objcopy -O ihex $(EXECUTABLE) "led_library.hex"
	@echo 'Finished building: $@'
	@echo ' '

led_library.srec: $(EXECUTABLE)
	@echo 'Invoking: GNU S-Record Generator'
	arm-none-eabi-objcopy -O srec $(EXECUTABLE) "led_library.srec"
	@echo 'Finished building: $@'
	@echo ' '

led_library.lst: $(EXECUTABLE)
	@echo 'Invoking: GNU Listing Generator'
	arm-none-eabi-objdump -h -S $(EXECUTABLE) > "led_library.lst"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean::
	-$(RM) $(MEMORYMAP)$(OBJS)$(C_DEPS)$(SRECFILES)$(IHEXFILES)$(LISTINGS)$(EXECUTABLE) led_library
	-@echo ' '

secondary-outputs: $(MEMORYMAP) $(IHEXFILES) $(SRECFILES) $(LISTINGS)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
