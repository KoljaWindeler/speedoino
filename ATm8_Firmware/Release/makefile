################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include Debug/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
FLASH_IMAGE += \
Watchdog.hex \

EEPROM_IMAGE += \
Watchdog.eep \

SIZEDUMMY += \
sizedummy \

AVRDUDEDUMMY += \
avrdudedummy \


# All Target
all: Watchdog.elf secondary-outputs

# Tool invocations
Watchdog.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++ -Wl,-Map,Watchdog.map,--cref -mmcu=atmega8 -o"Watchdog.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Watchdog.hex: Watchdog.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex Watchdog.elf  "Watchdog.hex"
	@echo 'Finished building: $@'
	@echo ' '

Watchdog.eep: Watchdog.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex Watchdog.elf  "Watchdog.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Watchdog.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega8 Watchdog.elf
	@echo 'Finished building: $@'
	@echo ' '

avrdudedummy: Watchdog.elf
	@echo 'Invoking: AVRDude'
	/usr/bin/avrdude -pm8 -cavrisp2 -Pusb -V -u -Uflash:w:Watchdog.hex:a -Ulfuse:w:0xe4:m -Uhfuse:w:0xff:m
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ELFS)$(FLASH_IMAGE)$(EEPROM_IMAGE)$(S_DEPS)$(CXX_DEPS)$(S_UPPER_DEPS)$(C++_DEPS)$(ASM_DEPS)$(CC_DEPS)$(AVRDUDEDUMMY)$(CPP_DEPS)$(C_UPPER_DEPS)$(SIZEDUMMY) Watchdog.elf
	-@echo ' '

secondary-outputs: $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY) $(AVRDUDEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
