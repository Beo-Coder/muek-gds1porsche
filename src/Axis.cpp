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

void Axis::setDigitalIndex(uint8_t index) {
    digitalIndex = index;
}

uint8_t Axis::getDigitalIndex() const {
    return digitalIndex;
}








uint16_t Axis::readSensor() const {
    return adc->readChannel(index);
}


long Axis::linearCalculation(uint16_t value) {
    if(value < calibrationData[2]){
        return map(value, calibrationData[0], calibrationData[2], -1028,0);
    }else{
        return map(value, calibrationData[2], calibrationData[1], 0,1028);
    }


}

double Axis::logCalculation(double value) {

    if (value < calibrationData[2]) {
        return -((0 - -1028) / (pow(1-(base[0]-1), calibrationData[2]) - pow(1-(base[0]-1), calibrationData[0])) *
                 (pow(1-(base[0]-1), -value+calibrationData[2]) - pow(1-(base[0]-1), calibrationData[0])));
    } else {
        return (1028 - 0) / (pow(1-(base[1]-1), calibrationData[1]) - pow(1-(base[1]-1), calibrationData[2])) *
               (pow(1-(base[1]-1), value) - pow(1-(base[1]-1), calibrationData[2])) + 0;
    }



}

double Axis::expCalculation(double value) {

    if (value < calibrationData[2]) {
        return -((0 - -1028) / (pow(base[0], calibrationData[2]) - pow(base[0], calibrationData[0])) *
           (pow(base[0], -value+calibrationData[2]) - pow(base[0], calibrationData[0])));
    } else {
        return (1028 - 0) / (pow(base[1], calibrationData[1]) - pow(base[1], calibrationData[2])) *
           (pow(base[1], value) - pow(base[1], calibrationData[2])) + 0;
    }

}

int8_t Axis::digitalCalculation(double value) {


    int8_t factor = 1;
    if(digitalCalibrationData[0] > digitalCalibrationData[1]) {
        factor = -1;
    }

    if((value > digitalCalibrationData[0] && value < digitalCalibrationData[1]) || (value > digitalCalibrationData[1] && value < digitalCalibrationData[0])){
        return 0;
    }else if(value > digitalCalibrationData[1]){
        return 1 * factor;
    }else if(value < digitalCalibrationData[0]){
        return -1 * factor;
    }
    return 0;
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
        value = linearCalculation(currentRawValue);
    }else if(mode == 1){
        // exponential
        value = long(expCalculation(double(currentRawValue)));
    }else if(mode == 2){
        // Logarithm
        value = long(logCalculation(double(currentRawValue)));
    }else if(mode == 3){
        //digital
        int8_t digitalValue = digitalCalculation(currentRawValue);
        value = digitalValue * 1028;
    }



    return value;
}

int8_t Axis::getDigitalValue() {

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


void Axis::initSettingsMenu(Navigator *navigator){
    settingsMenu = new Menu(navigator);

    settingsMenu->addItem("Back", &Navigator::navigatorMenuChangeStatic, 0);
    settingsMenu->addItem("Calibrate", &Axis::axisEntryAction, index * 10 + 1);
    settingsMenu->addItem("Base set", &Axis::axisEntryAction, index * 10 + 2);
    settingsMenu->addItem("Reset base", &Axis::axisEntryAction, index * 10 + 3);
    settingsMenu->addItem("Mode", &Axis::axisEntryAction, index * 10 + 4);
    settingsMenu->addItem("Digital Calibration", &Axis::axisEntryAction, index * 10 + 5);


}

void Axis::axisEntryAction(Navigator *navigator, uint8_t index) {
    uint8_t axisIndex = index / 10;
    switch (index % 10){
        case 1:
            navigator->input->axisCalibration(navigator->joystick->axis[axisIndex]);
            break;
        case 2:
            navigator->input->axisSetBase(navigator->joystick->axis[axisIndex]);
            break;
        case 3:
            navigator->joystick->axis[index/(10+5)]->calculateBase();
            break;
        case 4:
            navigator->input->axisSetMode(navigator->joystick->axis[axisIndex]);
            break;
        case 5:
            navigator->input->axisCalibrationDigital(navigator->joystick->axis[axisIndex]);

            break;
    }

}



