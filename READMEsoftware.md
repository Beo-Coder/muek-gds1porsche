# Porsche Mühk Software

# Table of Contents
1. [Intro](#intro)
2. [Overview](#overview)
3. [Emulating](#emulating-a-gamepadkeyboard)
4. [Memory Map](#eeprom-memory-map)


# Intro
This Readme file focuses on the software of Porsche Mühk.

# Overview
This file will not explain all aspects of the software.
Only important principles are explained.  

The software is written in C/C++ with Platform IO.
It can also be imported in the Arduino IDE.

I know that the software can certainly be made better and cleaner.


# Emulating a Gamepad/Keyboard

In Analog and Analog/Digital mode the Pico will emulate a gamepad.

Each axis on the device will be one axis on the gamepad.
Each button on the device will be one button on the gamepad.  
In Analog/Digital mode for each axis there are two extra buttons created.

To know which axis/button is which use a gamepad tester.
Example website I used for testing: [Gamepad tester](https://hardwaretester.com/gamepad)

For the Keyboard mode, each button has one key and each axis has two keys.  
The emulated Keyboard is an n-key rollover keyboard.  
To know which axis/button is which use a keyboard tester.
Example website I used for testing: [Keyboard tester](https://keyboard-test.space/)

The libraries used for emulating were written by me  
[Gamepad library](https://github.com/Beo-Coder/muehk-lib-JoystickHID)  
[Keyboard library]() -  Coming soon


# EEPROM Memory Map

All configuration data is stored on the external EEPROM chip.
The EEPROM I used has an address range from 0x0000 to 0x1FFF so 8191 bytes in total.

The EEPROM is divided into two different sections. 
A general section and the sections for all presets.

The general section is 20 bytes wide and stores the following:
- current preset
- 19 unused bytes

General section memory map:
<pre>
0x0000:  +-----------------------------------+
         | Current preset                    |
0x0001:  +-----------------------------------+
         | not used                          |
         .                                   .
         .                                   .
         |                                   |    
0x0014:  +-----------------------------------+
         |                                   |
         .                                   .
</pre>

The preset section contains all presets.
A preset is 405 bytes wide. So the reset of the EEPROM could theoretically store up to 20 presets.  

A preset contains the following data:
- 32 bytes per axis (2 bytes per calibration data, 2 bytes per digital calibration, 8 bytes per base, 1 bytes for the mode, 1 byte for 8 single bit data, 4 bytes not used)
- 2 bytes per button (1 byte for 8 single bit data, 1 byte not used)
- 5 byte for a general section (1 byte for axis count, 1 byte for button count, 1 byte for the joystick mode, 2 bytes not used)

In total a 405 byte wide preset can hold 10 axes and 40 buttons.  
With the unused bytes there is enough space to update/modify smaller things without doing a whole new memory map.  
Because the software normally only stores 10 presets 4121 bytes are free.
These bytes could be used to store more presets or for any other thing that need to be stored.

Preset memory map:

<pre>
0x0014:  +-----------------------------------+  Preset 0 General Section (5 bytes)
         | Axis count                        |
0x0015:  +-----------------------------------+
         | Button count                      |
0x0016:  +-----------------------------------+
         | Mode                              |
0x0017:  +-----------------------------------+
         | Not used                          |
         |                                   |
0x0019:  +-----------------------------------+  Axis 0 (32 bytes)
         | calibration data 0 MSB            |
0x001A:  +-----------------------------------+
         | calibration data 0 LSB            |
0x001B:  +-----------------------------------+
         | calibration data 1 MSB            |
0x001C:  +-----------------------------------+
         | calibration data 1 LSB            |
0x001D:  +-----------------------------------+
         | calibration data 2 MSB            |
0x001E:  +-----------------------------------+
         | calibration data 2 LSB            |
0x001F:  +-----------------------------------+
         | digital calibration data 0 MSB    |
0x0020:  +-----------------------------------+
         | digital calibration data 0 LSB    |
0x0021:  +-----------------------------------+
         | digital calibration data 1 MSB    |
0x0022:  +-----------------------------------+
         | digital calibration data 1 LSB    |
0x0023:  +-----------------------------------+
         | base 0                            |
         |                                   |
         |                                   |
         |                                   |
         |                                   |
         |                                   |
         |                                   |
         |                                   |
0x002B:  +-----------------------------------+
         | base 1                            |
         |                                   |
         |                                   |
         |                                   |
         |                                   |
         |                                   |
         |                                   |
         |                                   |
0x0033:  +-----------------------------------+
         | mode                              |
0x0034:  +-----------------------------------+
         | bit data                          |
0x0035:  +-----------------------------------+
         | Not used                          |
         |                                   |
         |                                   |
         |                                   |
0x0039:  +-----------------------------------+  Axis 1-9 (32*9 bytes)
         |                                   |
         .                                   .

         .                                   .
         |                                   |
0x0159:  +-----------------------------------+  Button 0 (2 bytes)
         | single bit data                   |
0x015A:  +-----------------------------------+ 
         | not used                          |
0x015B:  +-----------------------------------+  Button 1-39 (2*39 bytes)
         |                                   |
         .                                   .

         .                                   .
         |                                   |
0x01a9:  +-----------------------------------+  Preset 1 General Section ...
         |                                   |
         .                                   .
         .                                   .
</pre>


To read/write to the EEPROM I used my own library  
[EEPROM 24 lib](https://github.com/Beo-Coder/muehk-lib-Microchip-EEPROM-24)



( ._.)