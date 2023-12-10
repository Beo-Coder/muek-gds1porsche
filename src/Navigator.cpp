//
// Created by Leo on 11.04.2023.
//

#include "Arduino.h"
#include "Navigator.h"
#include "Display.h"
#include "Menu.h"
#include "Encoder.h"
#include "Input.h"
#include "MyJoystick.h"
#include "LiquidCrystal_I2C.h"
#include "MCP3204_MCP3208.h"
#include "EEPROM_Microchip_24.h"


Navigator::Navigator(Display *display, MyJoystick *joystick) {
    this->display = display;
    this->joystick = joystick;
    input = new Input(this);
    lcdBacklightTime = LCD_BACKLIGHT_OFF_TIME;


}

void Navigator::createMenus() {
    testMenu = new Menu(this);
    mainMenu = new Menu(this);
    settingsMenu = new Menu(this);

    settingsMenuPresets = new Menu(this);
    settingsMenuGeneral = new Menu(this);
    settingsMenuAxes = new Menu(this);
    settingsMenuButtons = new Menu(this);

    mainMenu->addItem("Porsche Muehk V" + String(VERSION));
    mainMenu->addItem("Settings", &navigatorMenuChangeStatic, 2);
    //mainMenu->addItem("Debug", &navigatorMenuChangeStatic, 3);
    mainMenu->addItem("Disable Output", &navigatorItemActionStatic, 0);


    settingsMenu->addItem("Back", &navigatorMenuChangeStatic, 0);
    settingsMenu->addItem("Set Preset", &navigatorItemActionStatic, 4);
    settingsMenu->addItem("Save Preset", &navigatorItemActionStatic, 1);
    settingsMenu->addItem("General", &navigatorMenuChangeStatic, 21);
    settingsMenu->addItem("Axes", &navigatorMenuChangeStatic, 22);
    settingsMenu->addItem("Buttons", &navigatorMenuChangeStatic, 23);
    settingsMenu->addItem("Reinit Joystick", &navigatorItemActionStatic, 199);
    settingsMenu->addItem("Reset Preset", &navigatorItemActionStatic, 2);
    settingsMenu->addItem("Factory Reset", &navigatorItemActionStatic, 3);


    settingsMenuGeneral->addItem("Back", &navigatorMenuChangeStatic, 0);

    settingsMenuGeneral->addItem("Number of Axes", &navigatorItemActionStatic, 200);
    settingsMenuGeneral->addItem("Number of Buttons", &navigatorItemActionStatic, 201);
    settingsMenuGeneral->addItem("Digital Axis Mode", &navigatorItemActionStatic, 202);


    settingsMenuAxes->addItem("Back", &navigatorMenuChangeStatic, 0);
    for (int i = 0; i < this->joystick->getAxisCount(); i++) {
        settingsMenuAxes->addItem(("Axis " + String(i)), &MyJoystick::axisEntryStatic, i);
        this->joystick->axis[i]->initSettingsMenu(this);
    }


    settingsMenuButtons->addItem("Back", &navigatorMenuChangeStatic, 0);
    for (int i = 0; i < this->joystick->getButtonCount(); i++) {
        settingsMenuButtons->addItem(("Button " + String(i)), &MyJoystick::buttonEntryStatic, i);
        this->joystick->button[i]->initSettingsMenu(this);
    }

}

void Navigator::init(Encoder *pEncoder) {
    this->encoder = pEncoder;
    createMenus();
    this->display->setNewMenu(mainMenu, false);
    menuHistory[menuHistoryPointer] = mainMenu;

}

void Navigator::setNextMenu(Menu *menu) {
    menuHistoryCursorPos[menuHistoryPointer] = display->currentCursorPos;
    menuHistoryScrollOffset[menuHistoryPointer] = display->scrollOffset;
    menuHistoryPointer++;
    menuHistory[menuHistoryPointer] = menu;

    this->display->setNewMenu(menu, false);
}

void Navigator::setPreviousMenu() {
    if (encoder->mode != 0) {
        encoder->mode = 0;
    } else {
        menuHistoryPointer--;
        display->currentCursorPos = menuHistoryCursorPos[menuHistoryPointer];
        display->scrollOffset = menuHistoryScrollOffset[menuHistoryPointer];
    }
    this->display->setNewMenu(menuHistory[menuHistoryPointer], true);


}

void Navigator::reprintMenu() const {
    this->display->printMenu();

}

void Navigator::checkEncoderFlag() {
    if (encoder->flag != 0) {
        lastUserInput = millis() / 1000;
        switch (encoder->mode) {
            case 0:
                display->setCursor(encoder->flag);
                break;
            case 1:
                input->changeValueInput(encoder->flag);
                break;
            case 2:
                input->changeSelectInput(encoder->flag);
                break;
            default:
                break;
        }
        encoder->resetEncoderFlag();
        delay(10);
    }

}

void Navigator::checkEncoderButtonFlag() {
    if (encoder->buttonFlag) {
        lastUserInput = millis() / 1000;
        if(!backlightDim){
            switch (encoder->mode) {
                case 0:
                    display->currentMenu->executeFunction(display->currentCursorPos + display->scrollOffset);
                    break;
                default:
                    break;
            }
        }

        encoder->buttonFlag = false;
    }

}

void Navigator::checkDisplay() {
    if (lcdBacklightTime != 0) {
        unsigned long currentTime = millis() / 1000;
        if ((currentTime - lastUserInput < lcdBacklightTime) && backlightDim) {
            display->lcd->backlight();
            backlightDim = false;

        } else if ((currentTime - lastUserInput > lcdBacklightTime) && !backlightDim) {
            display->lcd->noBacklight();
            backlightDim = true;
        }
    }


}

void Navigator::menuChange(uint8_t index) {
    switch (index) {
        case 0:
            setPreviousMenu();
            break;
        case 1:
            setNextMenu(mainMenu);
            break;
        case 2:
            setNextMenu(settingsMenu);
            break;
        case 3:
            setNextMenu(testMenu);
            break;
        case 20:
            setNextMenu(settingsMenuPresets);
            break;
        case 21:
            setNextMenu(settingsMenuGeneral);
            break;
        case 22:
            setNextMenu(settingsMenuAxes);
            break;
        case 23:
            setNextMenu(settingsMenuButtons);
            break;
        default:
            break;

    }
}


void Navigator::itemAction(uint8_t index) {

    switch (index) {
        case 0:
            joystick->toggleOutput();
            if (joystick->getOutputEnable()) {
                mainMenu->changeName(3, "Disable Output");
            } else {
                mainMenu->changeName(3, "Enable Output");
            }
            reprintMenu();
            break;
        case 1:
            input->generalSavePreset();
            break;
        case 2:
            input->resetPresetPrompt();

            break;
        case 3:
            input->factoryResetPrompt();

            break;
        case 4:
            input->generalSetPreset();
            itemAction(199);
            break;
        case 199:
#define AIRCR_Register (*((volatile uint32_t*)(PPB_BASE + 0x0ED0C)))
            AIRCR_Register = 0x5FA0004;
            break;
        case 200:
            input->generalSetAxisCount();
            break;
        case 201:
            input->generalSetButtonCount();
            break;
        case 202:
            input->generalSetMode();
            break;
        default:
            break;
    }

}


void Navigator::navigatorMenuChangeStatic(Navigator *navigator, uint8_t index) {
    navigator->menuChange(index);
}

void Navigator::navigatorItemActionStatic(Navigator *navigator, uint8_t index) {
    navigator->itemAction(index);

}

