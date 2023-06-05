
// Copyright (c) 2023. Leonhard Baschang


#ifndef UNTITLED19_BUTTON_H
#define UNTITLED19_BUTTON_H


class Button {

    bool toggleMode = false;
    bool normalOpen = true;

    bool currentState = false;
    bool toggleState = false;
    unsigned long lastPressed = 0;

public:

    Button();
    bool getState() const;
    bool stateChanged(uint8_t newState);

    void setToggleMode(bool pToggleMode);
    bool getToggleTMode() const;

    void setNormalOpen(bool pNormalOpen);
    bool getNormalOpen() const;


};


#endif //UNTITLED19_BUTTON_H
