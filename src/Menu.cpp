//
// Created by Leo on 11.04.2023.
//

#include "Arduino.h"
#include "Menu.h"

#include <utility>
#include "Navigator.h"


Menu::Menu(Navigator *navigator) {
    this->navigator = navigator;

}


void Menu::addItem(const String &name, void(*function)(Navigator *, uint8_t), uint8_t index) {
    itemName[itemCount] = name;
    itemFunction[itemCount] = function;
    itemFunctionIndex[itemCount] = index;
    itemCount++;

}

uint8_t Menu::getItemCount() const {
    return itemCount;
}

String Menu::getName(uint8_t index) {
    return itemName[index];
}

void Menu::executeFunction(uint8_t itemIndex) {
    if (itemFunction[itemIndex] != nullptr) {
        (*itemFunction[itemIndex])(navigator, itemFunctionIndex[itemIndex]);
    }

}

void Menu::changeName(uint8_t index, arduino::String name) {
    itemName[index] = std::move(name);
}




