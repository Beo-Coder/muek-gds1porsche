// Copyright (c) 2023. Leonhard Baschang

#ifndef GDS1PORSCHEMUEHK_DISPLAY_H
#define GDS1PORSCHEMUEHK_DISPLAY_H

#include "Menu.h"



class LiquidCrystal_I2C;
class Encoder;






class Display {




    uint8_t previousCursorPos = 0;



    void printCursor() const;


public:
    uint8_t lcdWidth;
    uint8_t lcdHeight;
    String blankLine;
    LiquidCrystal_I2C *lcd;
    Menu *currentMenu{};

    uint8_t currentCursorPos = 0;
    uint8_t scrollOffset = 0;
    Display(uint8_t lcdAddress, uint8_t lcdWidth, uint8_t lcdHeight);

    void init() const;
    void setNewMenu(Menu *menu, bool fromHistory);




    void setCursor(int8_t direction);
    void printMenu();


};


#endif //GDS1PORSCHEMUEHK_DISPLAY_H
