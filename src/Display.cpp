//
// Created by Leo on 11.04.2023.
//

#include "Arduino.h"
#include "Display.h"
#include "LiquidCrystal_I2C.h"
#include "Menu.h"
#include "Encoder.h"


Display::Display(uint8_t lcdAddress, uint8_t lcdWidth, uint8_t lcdHeight) {
    lcd = new LiquidCrystal_I2C(lcdAddress, lcdWidth, lcdHeight);
    this->lcdWidth = lcdWidth;
    this->lcdHeight = lcdHeight;

    for(int i=0; i<lcdWidth;i++){
        blankLine = blankLine + " ";
    }


}

void Display::init() {
    lcd->init();
    lcd->clear();
    lcd->backlight();
}


void  Display::setNewMenu(Menu *menu, bool fromHistory){
    if(!fromHistory){
        scrollOffset = 0;
        currentCursorPos = 0;
    }

    currentMenu = menu;
    printMenu();
}

void Display::printMenu() {

    lcd->clear();

    for (int i = 0; i < lcdHeight; i++) {
        lcd->setCursor(1, i);
        lcd->print(currentMenu->getName(i + scrollOffset));
    }
    printCursor();
}

void Display::printCursor() {
    lcd->setCursor(0, previousCursorPos);
    lcd->print(" ");
    lcd->setCursor(0, currentCursorPos);
    lcd->print(">");

}


void Display::setCursor(int8_t direction) {
    if (direction == 1 || direction == -1) {
        if (currentCursorPos + direction >= 0 && currentCursorPos + direction < lcdHeight) {
            previousCursorPos = currentCursorPos;
            currentCursorPos = currentCursorPos + direction;
            printCursor();
        } else if (scrollOffset + direction >= 0 &&
                   scrollOffset + direction < currentMenu->getItemCount() - (lcdHeight - 1)) {
            scrollOffset = scrollOffset + direction;
            printMenu();

        }

    }


}


