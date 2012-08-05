################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../inc/HardwareSerial.cpp \
../inc/I2C.cpp \
../inc/Print.cpp \
../inc/Sd2Card.cpp \
../inc/SdFat.cpp \
../inc/SdFatUtil.cpp \
../inc/SdFile.cpp \
../inc/SdStream.cpp \
../inc/SdVolume.cpp \
../inc/istream.cpp \
../inc/ostream.cpp \
../inc/ssd0323.cpp 

C_SRCS += \
../inc/pins_arduino.c \
../inc/wiring.c \
../inc/wiring_analog.c \
../inc/wiring_digital.c \
../inc/wiring_pulse.c \
../inc/wiring_shift.c 

OBJS += \
./inc/HardwareSerial.o \
./inc/I2C.o \
./inc/Print.o \
./inc/Sd2Card.o \
./inc/SdFat.o \
./inc/SdFatUtil.o \
./inc/SdFile.o \
./inc/SdStream.o \
./inc/SdVolume.o \
./inc/istream.o \
./inc/ostream.o \
./inc/pins_arduino.o \
./inc/ssd0323.o \
./inc/wiring.o \
./inc/wiring_analog.o \
./inc/wiring_digital.o \
./inc/wiring_pulse.o \
./inc/wiring_shift.o 

C_DEPS += \
./inc/pins_arduino.d \
./inc/wiring.d \
./inc/wiring_analog.d \
./inc/wiring_digital.d \
./inc/wiring_pulse.d \
./inc/wiring_shift.d 

CPP_DEPS += \
./inc/HardwareSerial.d \
./inc/I2C.d \
./inc/Print.d \
./inc/Sd2Card.d \
./inc/SdFat.d \
./inc/SdFatUtil.d \
./inc/SdFile.d \
./inc/SdStream.d \
./inc/SdVolume.d \
./inc/istream.d \
./inc/ostream.d \
./inc/ssd0323.d 


# Each subdirectory must supply rules for building sources it contributes
inc/%.o: ../inc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"/home/jkw/Store/17 - Speedmaster/Firmware/inc" -I"/home/jkw/Store/17 - Speedmaster/Firmware" -I/usr/lib/avr/include/ -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -fno-exceptions -v -lm -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

inc/%.o: ../inc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/home/jkw/Store/17 - Speedmaster/Firmware/inc" -I"/home/jkw/Store/17 - Speedmaster/Firmware" -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -v -lm -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


