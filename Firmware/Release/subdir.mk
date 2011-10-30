################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../blinker.cpp \
../clock_me.cpp \
../config.cpp \
../debug.cpp \
../display.cpp \
../dz.cpp \
../file_manager.cpp \
../fuel.cpp \
../gear.cpp \
../gps.cpp \
../helper.cpp \
../main.cpp \
../menu.cpp \
../oiler.cpp \
../reset.cpp \
../sd.cpp \
../sensors.cpp \
../speed.cpp \
../speedo.cpp \
../sprint.cpp \
../temperature.cpp \
../timer.cpp 

OBJS += \
./blinker.o \
./clock_me.o \
./config.o \
./debug.o \
./display.o \
./dz.o \
./file_manager.o \
./fuel.o \
./gear.o \
./gps.o \
./helper.o \
./main.o \
./menu.o \
./oiler.o \
./reset.o \
./sd.o \
./sensors.o \
./speed.o \
./speedo.o \
./sprint.o \
./temperature.o \
./timer.o 

CPP_DEPS += \
./blinker.d \
./clock_me.d \
./config.d \
./debug.d \
./display.d \
./dz.d \
./file_manager.d \
./fuel.d \
./gear.d \
./gps.d \
./helper.d \
./main.d \
./menu.d \
./oiler.d \
./reset.d \
./sd.d \
./sensors.d \
./speed.d \
./speedo.d \
./sprint.d \
./temperature.d \
./timer.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"/home/jkw/Store/17 - Speedmaster/Firmware/inc" -I"/home/jkw/Store/17 - Speedmaster/Firmware" -I/home/jkw/links/Speedmaster/ArduinoCore -I/usr/lib/avr/include/ -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -fno-exceptions -v -lm -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


