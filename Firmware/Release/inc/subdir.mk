################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../inc/EEPROM.cpp \
../inc/Sd2Card.cpp \
../inc/SdFat.cpp \
../inc/SdFatUtil.cpp \
../inc/SdFile.cpp \
../inc/SdStream.cpp \
../inc/SdVolume.cpp \
../inc/Wire.cpp \
../inc/istream.cpp \
../inc/ostream.cpp \
../inc/ssd0323.cpp 

C_SRCS += \
../inc/pins_arduino.c \
../inc/twi.c 

OBJS += \
./inc/EEPROM.o \
./inc/Sd2Card.o \
./inc/SdFat.o \
./inc/SdFatUtil.o \
./inc/SdFile.o \
./inc/SdStream.o \
./inc/SdVolume.o \
./inc/Wire.o \
./inc/istream.o \
./inc/ostream.o \
./inc/pins_arduino.o \
./inc/ssd0323.o \
./inc/twi.o 

C_DEPS += \
./inc/pins_arduino.d \
./inc/twi.d 

CPP_DEPS += \
./inc/EEPROM.d \
./inc/Sd2Card.d \
./inc/SdFat.d \
./inc/SdFatUtil.d \
./inc/SdFile.d \
./inc/SdStream.d \
./inc/SdVolume.d \
./inc/Wire.d \
./inc/istream.d \
./inc/ostream.d \
./inc/ssd0323.d 


# Each subdirectory must supply rules for building sources it contributes
inc/%.o: ../inc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I/home/jkw/links/eclipse_ws/ArduinoCore -I"/home/jkw/Store/17 - Speedmaster/Firmware/inc" -I"/home/jkw/Store/17 - Speedmaster/Firmware" -I/usr/lib/avr/include/ -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -fno-exceptions -v -lm -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

inc/%.o: ../inc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I/home/jkw/links/eclipse_ws/ArduinoCore -I"/home/jkw/Store/17 - Speedmaster/Firmware/inc" -I"/home/jkw/Store/17 - Speedmaster/Firmware" -I/usr/lib/avr/include/ -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -v -lm -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


