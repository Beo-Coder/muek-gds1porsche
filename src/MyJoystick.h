// Copyright (c) 2023. Leonhard Baschang

//
// Created by Leo on 07.04.2023.
//
#ifndef UNTITLED19_MYJOYSTICK_H
#define UNTITLED19_MYJOYSTICK_H

#include "Display.h"
#include "JoystickHID.h"
#include "KeyboardHID.h"
#include "header.h"

class Axis;

class Button;

class Menu;

class EEPROM_Microchip_24;

class MCP3204_MCP3208;


#define MAX_AXIS_COUNT 8
#define MAX_BUTTON_COUNT 24





#define EEPROM_GENERAL_ADDRESS_BEGIN 0x0000
#define EEPROM_GENERAL_BYTE_SIZE 20

#define EEPROM_PER_AXIS_BYTE_SIZE 32
#define EEPROM_PER_BUTTON_BYTE_SIZE 2

#define EEPROM_MAX_AXIS_COUNT 10
#define EEPROM_MAX_BUTTON_COUNT 40

#define EEPROM_PRESET_GENERAL_BYTE_SIZE 5
#define EEPROM_PRESET_AXES_BYTE_SIZE (EEPROM_PER_AXIS_BYTE_SIZE * EEPROM_MAX_AXIS_COUNT)
#define EEPROM_PRESET_BUTTONS_BYTE_SIZE (EEPROM_PER_BUTTON_BYTE_SIZE * EEPROM_MAX_BUTTON_COUNT)

#define EEPROM_PRESET_COUNT 10




class MyJoystick {

    JoystickHID *hid;
    KeyboardHID *keyboard;

    uint8_t preset{};
    uint8_t newPreset{};

    uint8_t joystickMode{};

    uint8_t newJoystickMode{};


    uint8_t axisCount{};
    uint8_t buttonCount{};

    uint8_t newAxisCount{};
    uint8_t newButtonCount{};

    uint8_t digitalAxisButtons{};

    // a,b,...,y,z,1,2,...,9,0,ä,ö,ü,,,.,-,+,#,^,ß,´,F1,F2,...,F12
    uint8_t keyboardModeKeycodes[60] = {0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                                        0x11, 0x12,
                                        0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1D, 0x1C, 0x1E, 0x1F,
                                        0x20,
                                        0x21, 0x22,
                                        0x23, 0x24, 0x25, 0x26, 0x27, 52, 51, 47, 54, 55, 56, 48, 50, 53, 45, 46, 0x64,
                                        0x3A, 0x3B, 0x3C, 0x3D, 0x3E,
                                        0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45};

    EEPROM_Microchip_24 *eeprom;
    MCP3204_MCP3208 *adc;

    uint8_t *buttonDemuxPins;
    uint8_t *buttonColumnPins;


    uint8_t buttonMatrixValue[BUTTON_MATRIX_ROWS][BUTTON_MATRIX_COLUMNS]{};

    void loadGeneralConfig();


    void loadPresetGeneralSettings(uint16_t addressBegin);
    void loadPresetAxisSettings(uint16_t addressBegin);
    void loadPresetButtonSettings(uint16_t addressBegin);




    void initAxis();

    void initButton();
    void initJoystick();

    void readButtonValues();


public:
    Axis *axis[MAX_AXIS_COUNT]{};
    Button *button[MAX_BUTTON_COUNT]{};


    MyJoystick(MCP3204_MCP3208 *adc, EEPROM_Microchip_24 *eeprom,
               uint8_t *buttonDemuxPins, uint8_t *buttonColumnPins);


    void begin();

    void enableOutput();

    void disableOutput();

    void toggleOutput();

    void updateAxis();

    void updateButton();

    uint8_t getAxisCount() const;

    uint8_t getButtonCount() const;


    uint8_t getNewAxisCount() const;

    uint8_t getNewButtonCount() const;

    void setNewAxisCount(uint8_t axisCount);

    void setNewButtonCount(uint8_t axisCount);

    uint8_t getJoystickMode() const;


    uint8_t getNewJoystickMode() const;

    uint8_t getPreset() const;

    void setNewJoystickMode(uint8_t mode);

    void setNewPreset(uint8_t presetIndex);

    uint8_t getOutputEnable() const;




    void axisEntry(Navigator *navigator, uint8_t index);

    static void axisEntryStatic(Navigator *navigator, uint8_t index);


    static void buttonEntryStatic(Navigator *navigator, uint8_t index);

    void buttonEntry(Navigator *navigator, uint8_t index);


    bool enable = true;

    void resetPreset(uint8_t presetNumber);
    void factoryReset();
    void storeGeneralConfig();
    void storePreset(uint8_t presetIndex);
};


#endif //UNTITLED19_MYJOYSTICK_H
