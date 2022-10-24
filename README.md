# Rpi Arduino Companion
Arduino companion for a Raspberry Pi, to reboot the Raspberry automatically if it has been unreachable for some minutes.

I did this because I configured the Raspberry Pi 3 to boot up from and usb ssd instead of the sd card.
The Raspberry Pi 3 boot is so bugged that once avery 3 times at boot the disc is not found, so often the Rpi requires a manual poweroff and reboot.
This makes the raspberry impossible to reboot when nobody is at home.

Now no more, the Raspberry is automatically rebooted by the Arduino when necessary.

## Hardware used

- Arduino Uno board ([original](https://store.arduino.cc/products/arduino-uno-rev3), [improved](https://www.keyestudio.com/products/keyestudio-uno-r3-motor-plus-development-board-for-arduinousb-cable))
- Ethernet shield ([original W5500](https://store.arduino.cc/collections/shields/products/arduino-ethernet-shield-2), [the one used W5100](https://www.keyestudio.com/products/keyestudio-w5100-ethernet-shield-board-with-sd-card-slot-original-chip-compatible-with-arduino-unor3-mega-2560r3-due-1280))

## Connection diagram

(diagram Work in progress, here soon™ )

- Raspberry GPIO pin 4: 5V coming directly from USB power, connect this to Arduino 5V pin, to power up the arduino from the Raspberry 
- Raspberry GPIO pin 6: GND, connect this to Arduino GND pin
- Raspberry pin RUN - is not among the GPIO pins and actually on the Rpi 3 needt to be soldered because it's missing - connect this to Arduino digital pin 7
