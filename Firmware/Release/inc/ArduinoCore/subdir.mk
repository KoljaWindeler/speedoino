################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../inc/ArduinoCore/HardwareSerial.cpp \
../inc/ArduinoCore/Print.cpp \
../inc/ArduinoCore/Tone.cpp \
../inc/ArduinoCore/WMath.cpp \
../inc/ArduinoCore/WString.cpp 

C_SRCS += \
../inc/ArduinoCore/WInterrupts.c \
../inc/ArduinoCore/pins_arduino.c \
../inc/ArduinoCore/wiring.c \
../inc/ArduinoCore/wiring_analog.c \
../inc/ArduinoCore/wiring_digital.c \
../inc/ArduinoCore/wiring_pulse.c \
../inc/ArduinoCore/wiring_shift.c 

OBJS += \
./inc/ArduinoCore/HardwareSerial.o \
./inc/ArduinoCore/Print.o \
./inc/ArduinoCore/Tone.o \
./inc/ArduinoCore/WInterrupts.o \
./inc/ArduinoCore/WMath.o \
./inc/ArduinoCore/WString.o \
./inc/ArduinoCore/pins_arduino.o \
./inc/ArduinoCore/wiring.o \
./inc/ArduinoCore/wiring_analog.o \
./inc/ArduinoCore/wiring_digital.o \
./inc/ArduinoCore/wiring_pulse.o \
./inc/ArduinoCore/wiring_shift.o 

C_DEPS += \
./inc/ArduinoCore/WInterrupts.d \
./inc/ArduinoCore/pins_arduino.d \
./inc/ArduinoCore/wiring.d \
./inc/ArduinoCore/wiring_analog.d \
./inc/ArduinoCore/wiring_digital.d \
./inc/ArduinoCore/wiring_pulse.d \
./inc/ArduinoCore/wiring_shift.d 

CPP_DEPS += \
./inc/ArduinoCore/HardwareSerial.d \
./inc/ArduinoCore/Print.d \
./inc/ArduinoCore/Tone.d \
./inc/ArduinoCore/WMath.d \
./inc/ArduinoCore/WString.d 


# Each subdirectory must supply rules for building sources it contributes
inc/ArduinoCore/%.o: ../inc/ArduinoCore/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"/home/jkw/Store/17 - Speedmaster/Firmware/inc" -I"/home/jkw/Store/17 - Speedmaster/Firmware" -I"/home/jkw/Store/04 - eclipse_ws/ArduinoCore" -I/usr/lib/avr/include/ -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -fno-exceptions -v -lm -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

inc/ArduinoCore/%.o: ../inc/ArduinoCore/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/home/jkw/Store/17 - Speedmaster/Firmware/inc" -I/home/jkw/links/eclipse_ws/ArduinoCore -I"/home/jkw/Store/17 - Speedmaster/Firmware" -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -v -lm -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


