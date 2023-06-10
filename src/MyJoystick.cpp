// Copyright (c) 2023. Leonhard Baschang


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
#include "KeyboardHID.h"


MyJoystick::MyJoystick(MCP3204_MCP3208 *adc, EEPROM_Microchip_24 *eeprom,
                       uint8_t *buttonDemuxPins, uint8_t *buttonColumnPins) {


    this->eeprom = eeprom;
    this->adc = adc;

    this->buttonDemuxPins = buttonDemuxPins;
    this->buttonColumnPins = buttonColumnPins;


    for (auto &i: buttonMatrixValue) {
        for (unsigned char &j: i) {
            j = 0;
        }
    }


    for (int i = 0; i < sizeof(&buttonDemuxPins) / sizeof(buttonDemuxPins[0]); i++) {
        pinMode(buttonDemuxPins[i], OUTPUT);
        digitalWrite(buttonDemuxPins[i], LOW);
    }

    for (int i = 0; i < sizeof(&buttonDemuxPins) / sizeof(buttonDemuxPins[0]); i++) {
        pinMode(buttonColumnPins[i], INPUT_PULLDOWN);
    }



    /*
    pinMode(25,OUTPUT);
    digitalWrite(25, HIGH);

    //resetPreset(0);
    //factoryReset();
    digitalWrite(25, LOW);
    */







    loadGeneralConfig();


    uint16_t presetAddressBegin = (EEPROM_GENERAL_ADDRESS_BEGIN + EEPROM_GENERAL_BYTE_SIZE) +
                                  ((EEPROM_PRESET_GENERAL_BYTE_SIZE + EEPROM_PRESET_AXES_BYTE_SIZE +
                                    EEPROM_PRESET_BUTTONS_BYTE_SIZE) * preset);
    loadPresetGeneralSettings(presetAddressBegin);

    initJoystick();

    loadPresetAxisSettings(presetAddressBegin + EEPROM_PRESET_GENERAL_BYTE_SIZE);
    loadPresetButtonSettings(presetAddressBegin + EEPROM_PRESET_GENERAL_BYTE_SIZE + EEPROM_PRESET_AXES_BYTE_SIZE);


}

void MyJoystick::loadGeneralConfig() {
    preset = eeprom->readByte(EEPROM_GENERAL_ADDRESS_BEGIN);
}

void MyJoystick::storeGeneralConfig() {
    uint8_t data[EEPROM_GENERAL_BYTE_SIZE];
    data[0] = newPreset;
    for (int i = 1; i < EEPROM_GENERAL_BYTE_SIZE; i++) {
        data[i] = 0;
    }
    eeprom->updatePage(EEPROM_GENERAL_ADDRESS_BEGIN, data, EEPROM_GENERAL_BYTE_SIZE);
}


void MyJoystick::loadPresetGeneralSettings(uint16_t addressBegin) {
    axisCount = eeprom->readByte(addressBegin);
    buttonCount = eeprom->readByte(addressBegin + 1);
    joystickMode = eeprom->readByte(addressBegin + 2);
}

void MyJoystick::loadPresetAxisSettings(uint16_t addressBegin) {
    for (int i = 0; i < axisCount; i++) {
        uint16_t axisAddressBegin = addressBegin + (i * EEPROM_PER_AXIS_BYTE_SIZE);

        uint16_t calibrationData[3];
        uint16_t digitalCalibrationData[2];
        double base[2];
        uint8_t mode;
        bool calibrateCenter;

        uint8_t *data = eeprom->readSequentialByte(axisAddressBegin, EEPROM_PER_AXIS_BYTE_SIZE);

        calibrationData[0] = (data[0] << 8) | data[1];
        calibrationData[1] = (data[2] << 8) | data[3];
        calibrationData[2] = (data[4] << 8) | data[5];

        digitalCalibrationData[0] = (data[6] << 8) | data[7];
        digitalCalibrationData[1] = (data[8] << 8) | data[9];

        uint8_t buffer[8];
        for (int j = 0; j < 8; j++) {
            buffer[j] = data[10 + j];
        }
        base[0] = *((double *) buffer);

        for (int j = 0; j < 8; j++) {
            buffer[j] = data[18 + j];
        }
        base[1] = *((double *) buffer);

        mode = data[26];

        calibrateCenter = data[27] & 0x01;


        axis[i]->setCalibrationData(calibrationData);
        axis[i]->setDigitalCalibrationData(digitalCalibrationData);
        axis[i]->setBase(base[0], base[1]);
        axis[i]->setMode(mode);
        axis[i]->setCalibrateCenter(calibrateCenter);


    }

}

void MyJoystick::loadPresetButtonSettings(uint16_t addressBegin) {
    for (int i = 0; i < buttonCount; i++) {
        uint16_t buttonAddressBegin = addressBegin + (i * EEPROM_PER_BUTTON_BYTE_SIZE);

        uint8_t *data = eeprom->readSequentialByte(buttonAddressBegin, EEPROM_PER_BUTTON_BYTE_SIZE);
        button[i]->setNormalOpen((data[0] >> 1) & 0x01);
        button[i]->setToggleMode(data[0] & 0x01);
    }
}

void MyJoystick::factoryReset() {
    //general section
    uint8_t generalSectionData[EEPROM_GENERAL_BYTE_SIZE];
    generalSectionData[0] = 0; // normal Preset
    for (int i = 1; i < EEPROM_GENERAL_BYTE_SIZE; i++) {
        generalSectionData[i] = 0;
    }
    eeprom->updatePage(EEPROM_GENERAL_ADDRESS_BEGIN, generalSectionData, EEPROM_GENERAL_BYTE_SIZE);
    for (int i = 0; i < EEPROM_PRESET_COUNT; i++) {
        resetPreset(i);
    }

}

void MyJoystick::resetPreset(uint8_t presetNumber) {
    uint16_t presetAddressBegin = (EEPROM_GENERAL_ADDRESS_BEGIN + EEPROM_GENERAL_BYTE_SIZE) +
                                  ((EEPROM_PRESET_GENERAL_BYTE_SIZE + EEPROM_PRESET_AXES_BYTE_SIZE +
                                    EEPROM_PRESET_BUTTONS_BYTE_SIZE) * presetNumber);

    // General section
    uint8_t generalData[5] = {FACTORY_AXIS_COUNT, FACTORY_BUTTON_COUNT, FACTORY_JOYSTICK_MODE, 0, 0};
    eeprom->updatePage(presetAddressBegin, generalData, 5);

    // Axes section
    uint16_t calibrationData[3] = {FACTORY_AXIS_CALIBRATION_MIN, FACTORY_AXIS_CALIBRATION_MAX,
                                   FACTORY_AXIS_CALIBRATION_CENTER};
    uint16_t digitalCalibrationData[2] = {FACTORY_AXIS_DIGITAL_CALIBRATION_LOWER,
                                          FACTORY_AXIS_DIGITAL_CALIBRATION_UPPER};
    double base[2] = {FACTORY_AXIS_BASE, FACTORY_AXIS_BASE};
    uint8_t mode = FACTORY_AXIS_MODE;
    bool calibrateCenter = FACTORY_AXIS_CALIBRATE_CENTER;

    uint8_t data[EEPROM_PER_AXIS_BYTE_SIZE];
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

    for (int i = 28; i < EEPROM_PER_AXIS_BYTE_SIZE; i++) {
        data[i] = 0;
    }
    for (int i = 0; i < EEPROM_MAX_AXIS_COUNT; i++) {
        eeprom->updatePage(presetAddressBegin + EEPROM_PRESET_GENERAL_BYTE_SIZE + (i * EEPROM_PER_AXIS_BYTE_SIZE), data,
                           EEPROM_PER_AXIS_BYTE_SIZE);
    }

    // Button section
    uint8_t buttonData[EEPROM_PER_BUTTON_BYTE_SIZE];
    buttonData[0] = (FACTORY_BUTTON_NORMAL_OPEN & 0x01) << 1 | (FACTORY_BUTTON_TOGGLE_MODE & 0x01);;
    for (int i = 1; i < EEPROM_PER_BUTTON_BYTE_SIZE; i++) {
        buttonData[i] = 0;
    }
    for (int i = 0; i < EEPROM_MAX_BUTTON_COUNT; i++) {
        eeprom->updatePage(presetAddressBegin + EEPROM_PRESET_GENERAL_BYTE_SIZE + EEPROM_PRESET_AXES_BYTE_SIZE +
                           (i * EEPROM_PER_BUTTON_BYTE_SIZE), buttonData, EEPROM_PER_BUTTON_BYTE_SIZE);
    }

}

void MyJoystick::storePreset() {
    uint16_t presetAddressBegin = (EEPROM_GENERAL_ADDRESS_BEGIN + EEPROM_GENERAL_BYTE_SIZE) +
                                  ((EEPROM_PRESET_GENERAL_BYTE_SIZE + EEPROM_PRESET_AXES_BYTE_SIZE +
                                    EEPROM_PRESET_BUTTONS_BYTE_SIZE) * preset);

    // General section
    uint8_t generalData[5] = {axisCount, buttonCount, joystickMode, 0, 0};
    eeprom->updatePage(presetAddressBegin, generalData, 5);

    // Axes section
    for (int i = 0; i < axisCount; i++) {
        uint16_t calibrationData[3];
        calibrationData[0] = axis[i]->getCalibrationData()[0];
        calibrationData[1] = axis[i]->getCalibrationData()[1];
        calibrationData[2] = axis[i]->getCalibrationData()[2];


        uint16_t digitalCalibrationData[2];
        digitalCalibrationData[0] = axis[i]->getDigitalCalibrationData()[0];
        digitalCalibrationData[1] = axis[i]->getDigitalCalibrationData()[1];

        double base[2];
        base[0] = axis[i]->getBase()[0];
        base[1] = axis[i]->getBase()[1];


        uint8_t mode = axis[i]->getMode();
        bool calibrateCenter = axis[i]->getCalibrateCenter();

        uint8_t data[EEPROM_PER_AXIS_BYTE_SIZE];
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

        uint8_t buffer[8];
        memcpy(buffer, (uint8_t *) &base[0], sizeof(base[0]));
        for (int j = 0; j < 8; j++) {
            data[10 + j] = buffer[j];
        }

        memcpy(buffer, (uint8_t *) &base[1], sizeof(base[1]));
        for (int j = 0; j < 8; j++) {
            data[18 + j] = buffer[j];
        }
        data[26] = mode;

        uint8_t bitData = 0x00 | (calibrateCenter & 0x01);
        data[27] = bitData;

        for (int j = 28; j < EEPROM_PER_AXIS_BYTE_SIZE; j++) {
            data[j] = 0;
        }

        eeprom->updatePage(presetAddressBegin + EEPROM_PRESET_GENERAL_BYTE_SIZE + (i * EEPROM_PER_AXIS_BYTE_SIZE), data,
                           EEPROM_PER_AXIS_BYTE_SIZE);
    }

    // Button section
    for (int i = 0; i < buttonCount; i++) {
        uint8_t buttonData[EEPROM_PER_BUTTON_BYTE_SIZE];
        buttonData[0] = (button[i]->getNormalOpen() & 0x01) << 1 | (button[i]->getToggleTMode() & 0x01);;
        for (int j = 1; j < EEPROM_PER_BUTTON_BYTE_SIZE; j++) {
            buttonData[j] = 0;
        }

        eeprom->updatePage(presetAddressBegin + EEPROM_PRESET_GENERAL_BYTE_SIZE + EEPROM_PRESET_AXES_BYTE_SIZE +
                           (i * EEPROM_PER_BUTTON_BYTE_SIZE), buttonData, EEPROM_PER_BUTTON_BYTE_SIZE);
    }

}

void MyJoystick::initJoystick() {

    if (axisCount > MAX_AXIS_COUNT) {
        axisCount = MAX_AXIS_COUNT;
    }

    for (uint8_t i = 0; i < axisCount; i++) {
        axis[i] = new Axis(i, adc);
    }


    if (buttonCount > MAX_BUTTON_COUNT) {
        buttonCount = MAX_BUTTON_COUNT;
    }


    for (uint8_t i = 0; i < buttonCount; i++) {
        button[i] = new Button();
    }


    newAxisCount = axisCount;
    newButtonCount = buttonCount;

    newJoystickMode = joystickMode;


    digitalAxisButtons = 0;


    initButton();
    initAxis();


    if (joystickMode == 2) {
        keyboard = new KeyboardHID;
    } else {
        hid = new JoystickHID(this->axisCount, this->buttonCount);
    }
}

uint8_t MyJoystick::getAxisCount() const {
    return axisCount;
}

uint8_t MyJoystick::getButtonCount() const {
    return buttonCount;
}

uint8_t MyJoystick::getNewAxisCount() const {
    return newAxisCount;
}

uint8_t MyJoystick::getNewButtonCount() const {
    return newButtonCount;
}

void MyJoystick::setNewAxisCount(uint8_t axisCount) {
    newAxisCount = axisCount;
}

void MyJoystick::setNewButtonCount(uint8_t buttonCount) {
    newButtonCount = buttonCount;
}

uint8_t MyJoystick::getJoystickMode() {
    return joystickMode;
}

uint8_t MyJoystick::getNewJoystickMode() {
    return newJoystickMode;
}

uint8_t MyJoystick::getPreset(){
    return preset;
}

void MyJoystick::setNewJoystickMode(uint8_t mode) {
    newJoystickMode = mode;
}

void MyJoystick::setNewPreset(uint8_t presetIndex) {
    newPreset = presetIndex;
}

void MyJoystick::initAxis() {

    for (int i = 0; i < axisCount; i++) {
        //loadAxisCalibration(i);

        //if in axis-digital-button Mode, add 2 Button per Axis
        if (joystickMode == 1) {
            if (axis[i]->getMode() == 3) {
                button[buttonCount] = new Button();
                buttonCount++;
                button[buttonCount] = new Button();
                buttonCount++;
                axis[i]->setDigitalIndex(digitalAxisButtons / 2);
                digitalAxisButtons += 2;


            }
        }


    }

}

void MyJoystick::initButton() {
    for (int i = 0; i < buttonCount; i++) {
        //loadButtonCalibration(i);
    }

}


void MyJoystick::begin() {


    if (joystickMode != 2) {
        hid->sendState();
    } else {
        keyboard->releaseAll();
    }


}

void MyJoystick::toggleOutput() {
    if (enable) {
        disableOutput();
    } else {
        enableOutput();
    }
}

void MyJoystick::disableOutput() {
    enable = false;
    if (joystickMode == 2) {
        keyboard->releaseAll();
    } else {
        for (int i = 0; i < axisCount; i++) {
            hid->setAxis(i, 0);
        }
        for (int i = 0; i < buttonCount; i++) {
            hid->setButton(i, 0);
        }
    }
}

void MyJoystick::enableOutput() {
    enable = true;
}


void MyJoystick::updateAxis() {
    if (enable) {
        if (joystickMode != 2) {
            for (int i = 0; i < axisCount; i++) {
                if (axis[i]->valueChanged()) {
                    int32_t value = axis[i]->getValue();
                    if (axis[i]->getMode() == 3 && joystickMode == 1) {
                        uint8_t button0Value = 0;
                        uint8_t button1Value = 0;

                        if (value == 1028) {
                            button0Value = 1;
                            button1Value = 0;
                        } else if (value == -1028) {
                            button0Value = 0;
                            button1Value = 1;
                        }
                        uint8_t digitalAxisOffset = axis[i]->getDigitalIndex();

                        hid->setButton((buttonCount - digitalAxisButtons) + (digitalAxisOffset * 2), button0Value);
                        hid->setButton((buttonCount - digitalAxisButtons) + (digitalAxisOffset * 2) + 1, button1Value);

                    } else {
                        hid->setAxis(i, value);
                    }


                }
            }
        } else {
            // Keyboard
            for (int i = 0; i < axisCount; i++) {
                if (axis[i]->valueChanged()) {
                    int8_t value = axis[i]->getDigitalValue();
                    if (value == 1) {
                        keyboard->release(keyboardModeKeycodes[i * 2 + 0], true);
                        keyboard->press(keyboardModeKeycodes[i * 2 + 1], true);
                    } else if (value == -1) {
                        keyboard->press(keyboardModeKeycodes[i * 2 + 0], true);
                        keyboard->release(keyboardModeKeycodes[i * 2 + 1], true);
                    } else {
                        keyboard->release(keyboardModeKeycodes[i * 2 + 0], true);
                        keyboard->release(keyboardModeKeycodes[i * 2 + 1], true);

                    }

                }


            }

        }

    }


}

void MyJoystick::readButtonValues() {
    for (uint8_t i = 0; i < BUTTON_MATRIX_ROWS; i++) {
        for (int j = 0; j < 3; j++) {
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
    if (enable) {
        readButtonValues();
        for (int i = 0; i < buttonCount - digitalAxisButtons; i++) {
            if (button[i]->stateChanged(buttonMatrixValue[i / BUTTON_MATRIX_COLUMNS][i % BUTTON_MATRIX_COLUMNS])) {
                if (joystickMode != 2) {
                    hid->setButton(i, button[i]->getState());
                } else {
                    if (button[i]->getState()) {
                        keyboard->press(keyboardModeKeycodes[axisCount * 2 + i], true);
                    } else {
                        keyboard->release(keyboardModeKeycodes[axisCount * 2 + i], true);
                    }
                }
            }
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

void MyJoystick::presetEntryStatic(Navigator *navigator, uint8_t index) {
    navigator->joystick->setNewPreset(index);
    navigator->input->reinitPrompt();
    navigator->input->returnToMenu();
    navigator->setPreviousMenu();

}





