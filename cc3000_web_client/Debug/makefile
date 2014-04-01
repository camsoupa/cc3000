################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include hal/CortexM3/subdir.mk
-include hal/CortexM3/GNU/subdir.mk
-include drivers/mss_uart/subdir.mk
-include drivers/mss_spi/subdir.mk
-include drivers/mss_i2c/subdir.mk
-include drivers/mss_gpio/subdir.mk
-include drivers/board/subdir.mk
-include CMSIS/subdir.mk
-include CMSIS/startup_gcc/subdir.mk
-include CC3000HostDriver/subdir.mk
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
cc3000_web_client.hex \

SRECFILES += \
cc3000_web_client.srec \

LISTINGS += \
cc3000_web_client.lst \


# All Target
all: cc3000_web_client secondary-outputs

# Tool invocations
cc3000_web_client: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU C Linker'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -specs=bare.specs -T../CMSIS/startup_gcc/debug-in-actel-smartfusion-esram.ld -Wl,-Map=$(EXECUTABLE).map -o"cc3000_web_client" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

memory-map.xml: $(EXECUTABLE)
	@echo 'Invoking: Memory map generator'
	actel-map -o "memory-map.xml" $(EXECUTABLE) $(BUILDCMD)
	@echo 'Finished building: $@'
	@echo ' '

cc3000_web_client.hex: $(EXECUTABLE)
	@echo 'Invoking: GNU Intel Hex File Generator'
	arm-none-eabi-objcopy -O ihex $(EXECUTABLE) "cc3000_web_client.hex"
	@echo 'Finished building: $@'
	@echo ' '

cc3000_web_client.srec: $(EXECUTABLE)
	@echo 'Invoking: GNU S-Record Generator'
	arm-none-eabi-objcopy -O srec $(EXECUTABLE) "cc3000_web_client.srec"
	@echo 'Finished building: $@'
	@echo ' '

cc3000_web_client.lst: $(EXECUTABLE)
	@echo 'Invoking: GNU Listing Generator'
	arm-none-eabi-objdump -h -S $(EXECUTABLE) > "cc3000_web_client.lst"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean::
	-$(RM) $(MEMORYMAP)$(OBJS)$(C_DEPS)$(SRECFILES)$(IHEXFILES)$(LISTINGS)$(EXECUTABLE) cc3000_web_client
	-@echo ' '

secondary-outputs: $(MEMORYMAP) $(IHEXFILES) $(SRECFILES) $(LISTINGS)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
