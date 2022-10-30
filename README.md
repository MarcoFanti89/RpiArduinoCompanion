# Rpi Arduino Companion
Arduino companion for a Raspberry Pi, to reboot the Raspberry automatically if it has been unreachable for some minutes.

I did this because I configured the Raspberry Pi 3 to boot up from and usb ssd instead of the sd card.
The Raspberry Pi 3 boot is so bugged that once avery 3 times at boot the disc is not found, so often the Rpi requires a manual poweroff and reboot.
This makes the raspberry impossible to reboot when nobody is at home.

Now no more, the Raspberry is automatically rebooted by the Arduino when necessary.

## Hardware used

- Arduino Uno board ([original](https://store.arduino.cc/products/arduino-uno-rev3), [improved](https://www.keyestudio.com/products/keyestudio-uno-r3-motor-plus-development-board-for-arduinousb-cable))
- Ethernet shield ([original W5500](https://store.arduino.cc/collections/shields/products/arduino-ethernet-shield-2), [the one used W5100](https://www.keyestudio.com/products/keyestudio-w5100-ethernet-shield-board-with-sd-card-slot-original-chip-compatible-with-arduino-unor3-mega-2560r3-due-1280))
- Relay module ([example](https://www.elegoo.com/products/elegoo-8-channel-relay-module-kit))in my case has to support 250 V AC

## Future developments
Use the same arduino to check connectivity with external world, and eventually reboot the router in the same way
