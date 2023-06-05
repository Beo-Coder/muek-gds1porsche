
// Copyright (c) 2023. Leonhard Baschang


#include "Arduino.h"
#include "Button.h"
//#include "EEPROM.h"
#include "header.h"


Button::Button() {
}

bool Button::getState() const {
    if (normalOpen) {
        return currentState;
    } else {
        return !currentState;
    }

}

bool Button::stateChanged(uint8_t newState) {

    if(millis() - lastPressed > BUTTON_DEBOUNCE_TIME){
        if (newState != currentState && !toggleMode) {
            currentState = newState;
            lastPressed = millis();
            return true;
        } else if (toggleMode) {


            if (newState && !currentState && !toggleState) {
                lastPressed = millis();
                currentState = true;
                toggleState = true;
            } else if (newState && currentState && !toggleState) {
                lastPressed = millis();
                currentState = false;
                toggleState = true;
            } else if(!newState){
                toggleState = false;

            }
            return true;

        } else {
            return false;
        }
    } else{
        return false;
    }

}

void Button::setToggleMode(bool pToggleMode) {
    this->toggleMode = pToggleMode;

}

bool Button::getToggleTMode() const {
    return toggleMode;
}

void Button::setNormalOpen(bool pNormalOpen) {
    this->normalOpen = pNormalOpen;

}

bool Button::getNormalOpen() const {
    return normalOpen;
}





