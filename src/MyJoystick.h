// Copyright (c) 2023. Leonhard Baschang

//
// Created by Leo on 07.04.2023.
//
#ifndef UNTITLED19_MYJOYSTICK_H
#define UNTITLED19_MYJOYSTICK_H
#include "Display.h"
#include "JoystickHID.h"
#include "header.h"

class Axis;
class Button;
class Menu;
class EEPROM_Microchip_24;
class MCP3204_MCP3208;


#define MAX_AXIS_COUNT 8
#define MAX_BUTTON_COUNT 24

#define EEPROM_ADDRESS_BEGIN_AXIS 0x0010
#define EEPROM_ADDRESS_RANGE_AXIS 28

#define EEPROM_ADDRESS_BEGIN_BUTTON 0x0100
#define EEPROM_ADDRESS_RANGE_BUTTON 1

class MyJoystick : public JoystickHID{




    uint8_t axisCount;
    uint8_t buttonCount;

    EEPROM_Microchip_24 *eeprom;
    MCP3204_MCP3208 *adc;

    uint8_t *buttonDemuxPins;
    uint8_t *buttonColumnPins;



    uint8_t buttonMatrixValue[BUTTON_MATRIX_ROWS][BUTTON_MATRIX_COLUMNS];

    void initAxis();

    void initButton();
    void readButtonValues();





public:
    Axis *axis[MAX_AXIS_COUNT];
    Button *button[MAX_BUTTON_COUNT];



    bool enable = true;
    MyJoystick(uint8_t axisCount, uint8_t buttonCount, MCP3204_MCP3208 *adc, EEPROM_Microchip_24 *eeprom, uint8_t *buttonDemuxPins, uint8_t *buttonColumnPins);


    void begin();

    void updateAxis();

    void updateButton();

    uint8_t getAxisCount() const;
    uint8_t getButtonCount() const;


    void axisEntry(Navigator *navigator, uint8_t index);
    static void axisEntryStatic(Navigator *navigator, uint8_t index);


    static void buttonEntryStatic(Navigator *navigator, uint8_t index);
    void buttonEntry(Navigator *navigator, uint8_t index);

    void storeAxisCalibration(uint8_t axisIndex);
    void loadAxisCalibration(uint8_t axisIndex);


    void storeButtonCalibration(uint8_t buttonIndex);
    void loadButtonCalibration(uint8_t buttonIndex);


    void storeBasicCalibration();
};


#endif //UNTITLED19_MYJOYSTICK_H
