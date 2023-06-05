// Copyright (c) 2023. Leonhard Baschang

//
// Created by Leo on 07.04.2023.
//
#include "Arduino.h"
#include "MyJoystick.h"
#include "JoystickHID.h"
#include "Axis.h"
#include "Button.h"
#include "Display.h"
#include "Input.h"
#include "Encoder.h"
#include "Menu.h"
#include "Navigator.h"
#include "EEPROM_Microchip_24.h"
#include "MCP3204_MCP3208.h"
#include "header.h"

MyJoystick::MyJoystick(uint8_t axisCount, uint8_t buttonCount, MCP3204_MCP3208 *adc, EEPROM_Microchip_24 *eeprom,
                       uint8_t *buttonDemuxPins, uint8_t *buttonColumnPins)
        : JoystickHID(axisCount, buttonCount) {

    this->eeprom = eeprom;
    this->adc = adc;

    this->buttonDemuxPins = buttonDemuxPins;
    this->buttonColumnPins = buttonColumnPins;

    for(int i=0; i<BUTTON_MATRIX_ROWS; i++){
        for(int j=0; j<BUTTON_MATRIX_COLUMNS; j++){
            buttonMatrixValue[i][j] = 0;
        }
    }


    for (int i = 0; i < sizeof(buttonDemuxPins)/sizeof(*buttonDemuxPins); i++) {
        pinMode(buttonDemuxPins[i], OUTPUT);
        digitalWrite(buttonDemuxPins[i], LOW);
    }

    for (int i = 0; i < sizeof(buttonDemuxPins)/sizeof(*buttonDemuxPins); i++) {
        pinMode(buttonColumnPins[i], INPUT_PULLDOWN);
    }


    if (axisCount > MAX_AXIS_COUNT) {
        axisCount = MAX_AXIS_COUNT;
    }

    if (buttonCount > MAX_BUTTON_COUNT) {
        buttonCount = MAX_BUTTON_COUNT;
    }


    for (uint8_t i = 0; i < axisCount; i++) {
        axis[i] = new Axis(i, adc);
    }


    for (uint8_t i = 0; i < buttonCount; i++) {
        button[i] = new Button();
    }


    this->axisCount = axisCount;
    this->buttonCount = buttonCount;


}

uint8_t MyJoystick::getAxisCount() const {
    return axisCount;
}

uint8_t MyJoystick::getButtonCount() const {
    return buttonCount;
}

void MyJoystick::initAxis() {
    for (int i = 0; i < axisCount; i++) {
        loadAxisCalibration(i);

    }

}

void MyJoystick::initButton() {
    for (int i = 0; i < buttonCount; i++) {
        loadButtonCalibration(i);
    }

}


void MyJoystick::begin() {
    initButton();
    initAxis();
    sendState();

}

void MyJoystick::storeAxisCalibration(uint8_t axisIndex) {
    uint16_t addressByte = EEPROM_ADDRESS_BEGIN_AXIS + (EEPROM_ADDRESS_RANGE_AXIS * axisIndex);

    uint16_t *calibrationData = axis[axisIndex]->getCalibrationData();
    uint16_t *digitalCalibrationData = axis[axisIndex]->getDigitalCalibrationData();
    double *base = axis[axisIndex]->getBase();
    uint8_t mode = axis[axisIndex]->getMode();
    bool calibrateCenter = axis[axisIndex]->getCalibrateCenter();

    uint8_t data[EEPROM_ADDRESS_RANGE_AXIS];
    data[0] = (calibrationData[0] >> 8) & 0xFF;
    data[1] = (calibrationData[0]) & 0xFF;

    data[2] = (calibrationData[1] >> 8) & 0xFF;
    data[3] = (calibrationData[1]) & 0xFF;

    data[4] = (calibrationData[2] >> 8) & 0xFF;
    data[5] = (calibrationData[2]) & 0xFF;

    data[6] = (digitalCalibrationData[0] >> 8) & 0xFF;
    data[7] = (digitalCalibrationData[0]) & 0xFF;

    data[8] = (digitalCalibrationData[1] >> 8) & 0xFF;
    data[9] = (digitalCalibrationData[1]) & 0xFF;



    // base data
    uint8_t buffer[8];
    memcpy(buffer, (uint8_t *) &base[0], sizeof(base[0]));
    for (int i = 0; i < 8; i++) {
        data[10 + i] = buffer[i];
    }

    memcpy(buffer, (uint8_t *) &base[1], sizeof(base[1]));
    for (int i = 0; i < 8; i++) {
        data[18 + i] = buffer[i];
    }
    data[26] = mode;


    uint8_t bitData = 0x00 | (calibrateCenter & 0x01);
    data[27] = bitData;
    eeprom->updatePage(addressByte, data, EEPROM_ADDRESS_RANGE_AXIS);


}

void MyJoystick::loadAxisCalibration(uint8_t axisIndex) {
    uint16_t addressByte = EEPROM_ADDRESS_BEGIN_AXIS + (EEPROM_ADDRESS_RANGE_AXIS * axisIndex);

    uint16_t calibrationData[3];
    uint16_t digitalCalibrationData[2];
    double base[2];
    uint8_t mode;
    bool calibrateCenter;

    uint8_t *data = eeprom->readSequentialByte(addressByte, EEPROM_ADDRESS_RANGE_AXIS);

    calibrationData[0] = (data[0] << 8) | data[1];
    calibrationData[1] = (data[2] << 8) | data[3];
    calibrationData[2] = (data[4] << 8) | data[5];

    digitalCalibrationData[0] = (data[6] << 8) | data[7];
    digitalCalibrationData[1] = (data[8] << 8) | data[9];

    uint8_t buffer[8];
    for (int i = 0; i < 8; i++) {
        buffer[i] = data[10 + i];
    }
    base[0] = *((double *) buffer);

    for (int i = 0; i < 8; i++) {
        buffer[i] = data[18 + i];
    }
    base[1] = *((double *) buffer);

    mode = data[26];

    calibrateCenter = data[27] & 0x01;

    axis[axisIndex]->setCalibrationData(calibrationData);
    axis[axisIndex]->setDigitalCalibrationData(digitalCalibrationData);
    axis[axisIndex]->setBase(base[0], base[1]);
    axis[axisIndex]->setMode(mode);
    axis[axisIndex]->setCalibrateCenter(calibrateCenter);

}

void MyJoystick::storeButtonCalibration(uint8_t buttonIndex) {
    uint16_t addressByte = EEPROM_ADDRESS_BEGIN_BUTTON + (EEPROM_ADDRESS_RANGE_BUTTON * buttonIndex);
    uint8_t data[EEPROM_ADDRESS_RANGE_BUTTON];
    data[0] = (button[buttonIndex]->getNormalOpen() & 0x01) << 1 | (button[buttonIndex]->getToggleTMode() & 0x01);
    eeprom->updatePage(addressByte, data, EEPROM_ADDRESS_RANGE_BUTTON);

}

void MyJoystick::loadButtonCalibration(uint8_t buttonIndex) {
    uint16_t addressByte = EEPROM_ADDRESS_BEGIN_BUTTON + (EEPROM_ADDRESS_RANGE_BUTTON * buttonIndex);
    uint8_t *data = eeprom->readSequentialByte(addressByte, EEPROM_ADDRESS_RANGE_BUTTON);
    button[buttonIndex]->setNormalOpen((data[0] >> 1) & 0x01);
    button[buttonIndex]->setToggleMode(data[0] & 0x01);

}

void MyJoystick::storeBasicCalibration(){
    for(int i=0; i<MAX_AXIS_COUNT; i++){
        uint16_t address = EEPROM_ADDRESS_BEGIN_AXIS + (EEPROM_ADDRESS_RANGE_AXIS * i);
        uint8_t data[EEPROM_ADDRESS_RANGE_AXIS];
        data[0] = (0 >> 8) & 0xFF;
        data[1] = (0) & 0xFF;

        data[2] = (4095 >> 8) & 0xFF;
        data[3] = (4095) & 0xFF;

        data[4] = ((4095/2) >> 8) & 0xFF;
        data[5] = ((4095/2)) & 0xFF;

        data[6] = (0 >> 8) & 0xFF;
        data[7] = (0) & 0xFF;

        data[8] = (4095 >> 8) & 0xFF;
        data[9] = (4095) & 0xFF;

        // base data

        double base[2] = {0,0};
        uint8_t buffer[8];
        memcpy(buffer, (uint8_t *) &base[0], sizeof(base[0]));
        for (int i = 0; i < 8; i++) {
            data[10 + i] = buffer[i];
        }

        memcpy(buffer, (uint8_t *) &base[1], sizeof(base[1]));
        for (int i = 0; i < 8; i++) {
            data[18 + i] = buffer[i];
        }

        data[26] = 0;


        uint8_t bitData = 0x00 | (0 & 0x01);
        data[27] = bitData;
        eeprom->updatePage(address, data, EEPROM_ADDRESS_RANGE_AXIS);
    }
    for(int i=0; i<MAX_BUTTON_COUNT; i++){
        uint16_t address = EEPROM_ADDRESS_BEGIN_BUTTON + (EEPROM_ADDRESS_RANGE_BUTTON * i);
        uint8_t data[EEPROM_ADDRESS_RANGE_BUTTON];
        data[0] = (1 & 0x01) << 1 | (0 & 0x01);
        eeprom->updatePage(address, data, EEPROM_ADDRESS_RANGE_AXIS);
    }

}


void MyJoystick::updateAxis() {
    for (int i = 0; i < axisCount; i++) {
        if (axis[i]->valueChanged()) {
            setAxis(i, axis[i]->getValue());
        }
    }

}
void MyJoystick::readButtonValues(){
    for (uint8_t i = 0; i < BUTTON_MATRIX_ROWS; i++) {
        for(int j=0; j<3; j++){
            digitalWrite(buttonDemuxPins[1 + j], ((i >> j) & 0x01));
        }
        // to bring the voltage back to 0 at all pins
        for (int j = 0; j < 10; j++) {
            __asm("nop");
        }
        for (int j = 0; j < BUTTON_MATRIX_COLUMNS; j++) {
            if (digitalRead(buttonColumnPins[j]) && buttonMatrixValue[i][j] == 0) {
                buttonMatrixValue[i][j] = 1;
            } else if ((!digitalRead(buttonColumnPins[j])) && buttonMatrixValue[i][j] == 1) {
                buttonMatrixValue[i][j] = 0;
            }
        }
    }
}

void MyJoystick::updateButton() {
    readButtonValues();
    for (int i = 0; i < buttonCount; i++) {
        if (button[i]->stateChanged(buttonMatrixValue[i/BUTTON_MATRIX_COLUMNS][i%BUTTON_MATRIX_COLUMNS])) {
            setButton(i, button[i]->getState());
        }
    }

}

void MyJoystick::axisEntry(Navigator *navigator, uint8_t index) {
    navigator->setNextMenu(axis[index]->settingsMenu);

}

void MyJoystick::axisEntryStatic(Navigator *navigator, uint8_t index) {
    navigator->joystick->axisEntry(navigator, index);

}

void MyJoystick::buttonEntry(Navigator *navigator, uint8_t index) {
    //navigator->setNextMenu(button[index]->settingsMenu);

}

void MyJoystick::buttonEntryStatic(Navigator *navigator, uint8_t index) {
    navigator->joystick->buttonEntry(navigator, index);

}





