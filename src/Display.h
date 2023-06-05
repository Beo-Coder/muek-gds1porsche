//
// Created by Leo on 11.04.2023.
//

#ifndef GDS1PORSCHEMUEHK_DISPLAY_H
#define GDS1PORSCHEMUEHK_DISPLAY_H

#include "Menu.h"



class LiquidCrystal_I2C;
class Encoder;


class Display {






    Encoder *encoder;


    uint8_t previousCursorPos = 0;



    void printCursor();
    void printMenu();

public:
    uint8_t lcdWidth;
    uint8_t lcdHeight;
    String blankLine;
    LiquidCrystal_I2C *lcd;
    Menu *currentMenu;

    uint8_t currentCursorPos = 0;
    uint8_t scrollOffset = 0;
    Display(uint8_t lcdAddress, uint8_t lcdWidth, uint8_t lcdHeight);

    void init();
    void setNewMenu(Menu *menu, bool fromHistory);




    void setCursor(int8_t direction);


};


#endif //GDS1PORSCHEMUEHK_DISPLAY_H
