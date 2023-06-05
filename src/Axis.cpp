
// Copyright (c) 2023. Leonhard Baschang


#include "Arduino.h"
//#include "EEPROM.h"
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

    base[0] = 1.001;
    base[1] = 1.001;

}

uint8_t Axis::getIndex() const{
    return index;
}


double * Axis::getBase() {
    return base;
}

void Axis::setCalibrationData(const uint16_t *pCalibrationData) {
    for(int i=0; i<3;i++){
        calibrationData[i] = pCalibrationData[i];
    }


}

uint16_t * Axis::getCalibrationData() {
    return calibrationData;
}

void Axis::setDigitalCalibrationData(const uint16_t *pDigitalCalibrationData) {
    for(int i=0; i<2;i++){
        digitalCalibrationData[i] = pDigitalCalibrationData[i];
    }

}

uint16_t * Axis::getDigitalCalibrationData() {
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






uint16_t Axis::readSensor() const {
    return adc->readChannel(index);
}

double Axis::logCalculation(double value) {
    if (value < calibrationData[2]) {
        return ((0 - -1028) / (pow(base[0], calibrationData[2]) - pow(base[0], calibrationData[0]))) *
               (pow(base[0], value) - pow(base[0], calibrationData[0])) + -1028;
    } else {
        return ((1028 - 0) / (pow(base[1], calibrationData[1]) - pow(base[1], calibrationData[2]))) *
               (pow(base[1], value) - pow(base[1], calibrationData[2])) + 0;
    }


}

void Axis::calculateBase() {
    base[0] = pow(1028, 1 / double(calibrationData[1] - calibrationData[2]));
    base[1] = pow(1028, 1 / double(calibrationData[2]));
}




void Axis::setBase(double pBase1, double pBase2) {
    base[0] = pBase1;
    base[1] = pBase2;
}


int32_t Axis::getValue() {
    int32_t value = 0;

    if(mode == 0){
        // linear
        value = map(currentRawValue, calibrationData[0], calibrationData[1], -1028, 1028);
    }else if(mode == 1 || mode == 2){
        // exponential || Logarithm
        value = long(logCalculation(double(currentRawValue)));

    }


    return value;
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


void Axis::initSettingsMenu(Navigator *navigator){
    settingsMenu = new Menu(navigator);

    settingsMenu->addItem("Back", &Navigator::navigatorMenuChangeStatic, 0);
    settingsMenu->addItem("Save Settings", &Axis::axisEntryAction, index * 10 + 0);
    settingsMenu->addItem("Calibrate", &Axis::axisEntryAction, index * 10 + 1);
    settingsMenu->addItem("Calibrate center?", &Axis::axisEntryAction, index * 10 + 2);
    settingsMenu->addItem("Base set", &Axis::axisEntryAction, index * 10 + 3);
    settingsMenu->addItem("Reset base", &Axis::axisEntryAction, index * 10 + 4);
    settingsMenu->addItem("Mode", &Axis::axisEntryAction, index * 10 + 5);
    settingsMenu->addItem("Digital Calibration", &Axis::axisEntryAction, index * 10 + 6);


}

void Axis::axisEntryAction(Navigator *navigator, uint8_t index) {
    uint8_t axisIndex = index / 10;
    switch (index % 10){
        case 0:
            navigator->joystick->storeAxisCalibration(axisIndex);
            break;
        case 1:
            navigator->input->axisCalibration(navigator->joystick->axis[axisIndex]);
            break;
        case 2:
            navigator->input->axisCenterCalibrationPrompt(navigator->joystick->axis[axisIndex]);
            break;
        case 3:
            navigator->input->axisSetBase(navigator->joystick->axis[axisIndex]);
            break;
        case 4:
            navigator->joystick->axis[index/(10+5)]->calculateBase();
            break;
        case 5:
            navigator->input->axisSetMode(navigator->joystick->axis[axisIndex]);
            break;
        case 6:
            navigator->input->axisCalibrationDigital(navigator->joystick->axis[axisIndex]);

            break;
    }

}



