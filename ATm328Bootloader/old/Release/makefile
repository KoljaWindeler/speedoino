################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
ATm328Bootloader.lss \

FLASH_IMAGE += \
ATm328Bootloader.hex \

EEPROM_IMAGE += \
ATm328Bootloader.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: ATm328Bootloader.elf secondary-outputs

# Tool invocations
ATm328Bootloader.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,ATm328Bootloader.map -mmcu=atmega16 -o "ATm328Bootloader.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

ATm328Bootloader.lss: ATm328Bootloader.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S ATm328Bootloader.elf  >"ATm328Bootloader.lss"
	@echo 'Finished building: $@'
	@echo ' '

ATm328Bootloader.hex: ATm328Bootloader.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex ATm328Bootloader.elf  "ATm328Bootloader.hex"
	@echo 'Finished building: $@'
	@echo ' '

ATm328Bootloader.eep: ATm328Bootloader.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex ATm328Bootloader.elf  "ATm328Bootloader.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: ATm328Bootloader.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega16 ATm328Bootloader.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) ATm328Bootloader.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
