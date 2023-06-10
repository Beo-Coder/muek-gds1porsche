//
// Created by Leo on 11.04.2023.
//

#ifndef GDS1PORSCHEMUEHK_NAVIGATOR_H
#define GDS1PORSCHEMUEHK_NAVIGATOR_H

#include "Menu.h"

class Display;

class Encoder;

class Input;

class MyJoystick;



class Navigator {


    Menu *mainMenu;
    Menu *settingsMenu;
    Menu *testMenu;

    Menu *settingsMenuPresets;
    Menu *settingsMenuGeneral;
    Menu *settingsMenuAxes;
    Menu *settingsMenuButtons;

    Menu *menuHistory[10];
    uint8_t menuHistoryCursorPos[10];
    uint8_t menuHistoryScrollOffset[10];
    uint8_t menuHistoryPointer = 0;







    void menuChange(uint8_t index);
    void itemAction(uint8_t index);


    void createMenus();
    void reprintMenu() const;

    unsigned long lcdBacklightTime;
    unsigned long lastUserInput = 0;
    bool backlightDim = false;






public:
    Input *input;
    MyJoystick *joystick;
    Display *display;
    Encoder *encoder;
    explicit Navigator(Display *display, MyJoystick *joystick);


    void init(Encoder *pEncoder);


    void setNextMenu(Menu *menu);
    void setPreviousMenu();





    void checkEncoderFlag();
    void checkEncoderButtonFlag();
    void checkDisplay();


    static void navigatorMenuChangeStatic(Navigator *navigator, uint8_t index);
    static void navigatorItemActionStatic(Navigator *navigator, uint8_t index);



};


#endif //GDS1PORSCHEMUEHK_NAVIGATOR_H
