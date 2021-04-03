#! /usr/bin/bash
# Script to call avrdude and flash the firmware.
# For some reason, the flash checking of platformio does not work. That is
# why avrdude is called via the terminal, which seems more robust.

# Set Options depending on operating system
if [[ "$OSTYPE" == "linux-gnu" ]]
then
    DUDE=avrdude
    DUDE_CONFIGURATION=/etc/avrdude.conf
else
    DUDE=C:/SysGCC/avr/bin/avrdude.exe 
    DUDE_CONFIGURATION=C:/SysGCC/avr/bin/avrdude.conf 
fi
DEVICE=m328p 
PORT=usb 
PROGRAMMER=atmelice_isp 
SPEED=10 
FIRMWARE=flash:w:./01_Code/.pio/build/uno/firmware.hex:a 

# Perform flash programming
$DUDE -C $DUDE_CONFIGURATION -p $DEVICE -P $PORT -c $PROGRAMMER -B $SPEED -U $FIRMWARE
