################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../main.c \
../reset.c \
../sm_driver.c \
../speed_cntr.c \
../uart.c 

OBJS += \
./main.o \
./reset.o \
./sm_driver.o \
./speed_cntr.o \
./uart.o 

C_DEPS += \
./main.d \
./reset.d \
./sm_driver.d \
./speed_cntr.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/home/jkw/Store/17 - Speedmaster/ATm328_Firmware" -I/usr/lib/avr/include/ -Wall -Werror -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


