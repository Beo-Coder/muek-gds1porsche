
// Copyright (c) 2023. Leonhard Baschang


#include "Arduino.h"
#include "Button.h"
#include "Navigator.h"
#include "MyJoystick.h"
#include "Input.h"
#include "header.h"


Button::Button(uint8_t pIndex) {
    this->index = pIndex;
}

bool Button::getState() const {
    if (normalOpen) {
        return currentState;
    } else {
        return !currentState;
    }

}

bool Button::stateChanged(uint8_t newState) {

    if (millis() - lastPressed > BUTTON_DEBOUNCE_TIME) {
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
            } else if (!newState) {
                toggleState = false;

            }
            return true;

        } else {
            return false;
        }
    } else {
        return false;
    }

}

void Button::setToggleMode(bool pToggleMode) {
    this->toggleMode = pToggleMode;

}

bool Button::getTogglMode() const {
    return toggleMode;
}

void Button::setNormalOpen(bool pNormalOpen) {
    this->normalOpen = pNormalOpen;

}

bool Button::getNormalOpen() const {
    return normalOpen;
}

void Button::initSettingsMenu(Navigator *navigator) {
    settingsMenu = new Menu(navigator);

    settingsMenu->addItem("Back", &Navigator::navigatorMenuChangeStatic, 0);
    settingsMenu->addItem("Mode", &Button::buttonEntryAction, index * 10 + 1);
    settingsMenu->addItem("Type", &Button::buttonEntryAction, index * 10 + 2);


}

void Button::buttonEntryAction(Navigator *navigator, uint8_t index) {
    uint8_t buttonIndex = index / 10;
    switch (index % 10) {
        case 1:
            navigator->input->buttonMode(navigator->joystick->button[buttonIndex]);
            break;
        case 2:
            navigator->input->buttonType(navigator->joystick->button[buttonIndex]);
            break;

    }

}




