################################################################################
# Automatically-generated file. Do not edit!
################################################################################

O_SRCS := 
C_SRCS := 
S_UPPER_SRCS := 
S_SRCS := 
OBJ_SRCS := 
MEMORYMAP := 
OBJS := 
C_DEPS := 
SRECFILES := 
IHEXFILES := 
LISTINGS := 
EXECUTABLE := 

# Every subdirectory with source files must be described here
SUBDIRS := \
. \
drivers/rgb_led_driver \
drivers/mss_gpio \
drivers/fpga_timer \
CMSIS \
CMSIS/startup_gcc \

################################################################################
# Microsemi SoftConsole IDE Variables
################################################################################

BUILDCMD = arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -specs=bare.specs -T../CMSIS/startup_gcc/debug-in-actel-smartfusion-esram.ld -Wl,-Map=$(EXECUTABLE).map 
SHELL := cmd.exe
EXECUTABLE := led_library

# Target-specific items to be cleaned up in the top directory.
clean::
	-$(RM) $(wildcard ./*.map) 
