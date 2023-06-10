
// Copyright (c) 2023. Leonhard Baschang


#ifndef UNTITLED19_BUTTON_H
#define UNTITLED19_BUTTON_H

#include "Menu.h"


class Button {

    bool toggleMode = false;
    bool normalOpen = true;

    bool currentState{};
    bool toggleState = false;
    unsigned long lastPressed = 0;

    uint8_t index;

public:

    explicit Button(uint8_t pIndex);


    bool getState() const;
    bool stateChanged(uint8_t newState);

    void setToggleMode(bool pToggleMode);
    bool getTogglMode() const;

    void setNormalOpen(bool pNormalOpen);
    bool getNormalOpen() const;



    Menu *settingsMenu{};
    void initSettingsMenu(Navigator *navigator);
    static void buttonEntryAction(Navigator *navigator, uint8_t index);



};


#endif //UNTITLED19_BUTTON_H
