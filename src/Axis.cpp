// Copyright (c) 2023. Leonhard Baschang


#include "Arduino.h"
#include "Axis.h"
#include "Menu.h"
#include "Navigator.h"
#include "Input.h"
#include "MyJoystick.h"
#include "MCP3204_MCP3208.h"


Axis::Axis(uint8_t index, MCP3204_MCP3208 *adc) {

    this->index = index;
    this->adc = adc;


    currentRawValue = 0;
    previousRawValue = 1;

    calibrationData[0] = FACTORY_AXIS_CALIBRATION_MIN;
    calibrationData[1] = FACTORY_AXIS_CALIBRATION_MAX;
    calibrationData[2] = FACTORY_AXIS_CALIBRATION_CENTER;

    digitalCalibrationData[0] = FACTORY_AXIS_DIGITAL_CALIBRATION_LOWER;
    digitalCalibrationData[1] = FACTORY_AXIS_DIGITAL_CALIBRATION_UPPER;


    base[0] = FACTORY_AXIS_BASE;
    base[1] = FACTORY_AXIS_BASE;


}

uint8_t Axis::getIndex() const {
    return index;
}


double *Axis::getBase() {
    return base;
}

void Axis::setCalibrationData(const uint16_t *pCalibrationData) {
    for (int i = 0; i < 3; i++) {
        calibrationData[i] = pCalibrationData[i];
    }


}

uint16_t *Axis::getCalibrationData() {
    return calibrationData;
}

void Axis::setDigitalCalibrationData(const uint16_t *pDigitalCalibrationData) {
    for (int i = 0; i < 2; i++) {
        digitalCalibrationData[i] = pDigitalCalibrationData[i];
    }

}

uint16_t *Axis::getDigitalCalibrationData() {
    return digitalCalibrationData;
}

void Axis::setCalibrateCenter(bool pCalibrateCenter) {
    calibrateCenter = pCalibrateCenter;
}

bool Axis::getCalibrateCenter() const {
    return calibrateCenter;
}

void Axis::setMode(uint8_t pMode) {
    mode = pMode;
}

uint8_t Axis::getMode() const {
    return mode;
}

void Axis::setDigitalIndex(uint8_t pIndex) {
    digitalIndex = pIndex;
}

uint8_t Axis::getDigitalIndex() const {
    return digitalIndex;
}


uint16_t Axis::readSensor() const {
    return adc->readChannel(adcIndexChannel[index]);
}


long Axis::linearCalculation(uint16_t value) {

    bool inverted = calibrationData[0] > calibrationData[1];

    if ((value < calibrationData[2] && !inverted) || (value > calibrationData[2] && inverted)) {
        return map(value, long(calibrationData[0]), long(calibrationData[2]), JOYSTICK_MIN_VALUE,
                   JOYSTICK_CENTER_VALUE);
    } else if ((value > calibrationData[2] && !inverted) || (value < calibrationData[2] && inverted)) {
        return map(value, long(calibrationData[2]), long(calibrationData[1]), JOYSTICK_CENTER_VALUE,
                   JOYSTICK_MAX_VALUE);
    }


}


double Axis::expCalculation(double value) {

    bool inverted = calibrationData[0] > calibrationData[1];


    if ((value < calibrationData[2] && !inverted) || (value > calibrationData[2] && inverted)) {
        return (pow((value - calibrationData[2]) / (calibrationData[0] - calibrationData[2]),
                    tan((PI / 4) * (base[0] + 1))) * (JOYSTICK_MIN_VALUE - JOYSTICK_CENTER_VALUE) +
                JOYSTICK_CENTER_VALUE);
    } else if ((value > calibrationData[2] && !inverted) || (value < calibrationData[2] && inverted)) {
        return (pow((value - calibrationData[2]) / (calibrationData[1] - calibrationData[2]),
                    tan((PI / 4) * (base[1] + 1))) * (JOYSTICK_MAX_VALUE - JOYSTICK_CENTER_VALUE) +
                JOYSTICK_CENTER_VALUE);
    }

}

int Axis::digitalCalculation(double value) {


    int8_t factor = 1;
    if (digitalCalibrationData[0] > digitalCalibrationData[1]) {
        factor = -1;
    }

    if ((value > digitalCalibrationData[0] && value < digitalCalibrationData[1]) ||
        (value > digitalCalibrationData[1] && value < digitalCalibrationData[0])) {
        return 0;
    } else if (value > digitalCalibrationData[1]) {
        return 1 * factor;
    } else if (value < digitalCalibrationData[0]) {
        return -1 * factor;
    }
    return 0;
}


void Axis::setBase(double pBase1, double pBase2) {
    base[0] = pBase1;
    base[1] = pBase2;
}


int32_t Axis::getValue() {
    int32_t value = 0;

    if (mode == 0) {
        // linear
        value = linearCalculation(currentRawValue);
    } else if (mode == 1) {
        // exponential
        value = long(expCalculation(double(currentRawValue)));
    } else if (mode == 2) {
        //digital
        auto digitalValue = int8_t(digitalCalculation(currentRawValue));
        value = digitalValue * JOYSTICK_MAX_VALUE;
    }


    return value;
}

int Axis::getDigitalValue() {

    return digitalCalculation(currentRawValue);
}

bool Axis::valueChanged() {
    currentRawValue = readSensor();
    if (currentRawValue != previousRawValue) {
        previousRawValue = currentRawValue;
        return true;
    } else {
        return false;
    }
}


void Axis::initSettingsMenu(Navigator *navigator) {
    settingsMenu = new Menu(navigator);

    settingsMenu->addItem("Back", &Navigator::navigatorMenuChangeStatic, 0);
    settingsMenu->addItem("Calibrate", &Axis::axisEntryAction, index * 10 + 1);
    settingsMenu->addItem("Base set", &Axis::axisEntryAction, index * 10 + 2);
    settingsMenu->addItem("Mode", &Axis::axisEntryAction, index * 10 + 3);
    settingsMenu->addItem("Digital Calibration", &Axis::axisEntryAction, index * 10 + 4);


}

void Axis::axisEntryAction(Navigator *navigator, uint8_t index) {
    uint8_t axisIndex = index / 10;
    switch (index % 10) {
        case 1:
            navigator->input->axisCalibration(navigator->joystick->axis[axisIndex]);
            break;
        case 2:
            navigator->input->axisSetBase(navigator->joystick->axis[axisIndex]);
            break;
        case 3:
            navigator->input->axisSetMode(navigator->joystick->axis[axisIndex]);
            break;
        case 4:
            navigator->input->axisCalibrationDigital(navigator->joystick->axis[axisIndex]);
            break;
    }

}



