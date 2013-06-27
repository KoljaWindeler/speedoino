#!/bin/bash
VERSION=`git --git-dir=$1../.git/ describe --tags 2>/dev/null`;
avr-objcopy -R .eeprom -O ihex $1"Release/Speedoino.elf" $1"Release/"$VERSION".hex"
