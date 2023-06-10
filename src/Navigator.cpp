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
#include "Axis.h"
#include "MCP3204_MCP3208.h"
#include "EEPROM_Microchip_24.h"



Navigator::Navigator(Display *display, MyJoystick *joystick) {
    this->display = display;
    this->joystick = joystick;
    input = new Input(this);




}

void Navigator::createMenus() {
    testMenu = new Menu(this);
    mainMenu = new Menu(this);
    settingsMenu = new Menu(this);

    settingsMenuPresets = new Menu(this);
    settingsMenuGeneral = new Menu(this);
    settingsMenuAxes = new Menu(this);
    settingsMenuButtons = new Menu(this);

    mainMenu->addItem("Porsche Muehk");
    mainMenu->addItem("Settings", &navigatorMenuChangeStatic, 2);
    mainMenu->addItem("Debug", &navigatorMenuChangeStatic, 3);
    mainMenu->addItem("Dis/Enable Output", &navigatorItemActionStatic, 0);


    settingsMenu->addItem("Back", &navigatorMenuChangeStatic, 0);
    settingsMenu->addItem("Save current Preset", &navigatorItemActionStatic, 1);
    settingsMenu->addItem("Set Preset", &navigatorMenuChangeStatic, 20);
    settingsMenu->addItem("General", &navigatorMenuChangeStatic, 21);
    settingsMenu->addItem("Axes", &navigatorMenuChangeStatic, 22);
    settingsMenu->addItem("Buttons", &navigatorMenuChangeStatic, 23);
    settingsMenu->addItem("Reinit Joystick", &navigatorItemActionStatic, 199);
    settingsMenu->addItem("Reset Preset", &navigatorItemActionStatic, 2);
    settingsMenu->addItem("Factory Reset", &navigatorItemActionStatic, 3);

    settingsMenuPresets->addItem("Back", &navigatorMenuChangeStatic, 0);
    for(int i=0; i<EEPROM_PRESET_COUNT; i++){
        settingsMenuPresets->addItem("Preset " + String(i), &MyJoystick::presetEntryStatic, i);
    }

    settingsMenuGeneral->addItem("Back", &navigatorMenuChangeStatic, 0);

    settingsMenuGeneral->addItem("Number of Axes", &navigatorItemActionStatic, 200);
    settingsMenuGeneral->addItem("Number of Buttons", &navigatorItemActionStatic, 201);
    settingsMenuGeneral->addItem("Mode", &navigatorItemActionStatic, 202);



    settingsMenuAxes->addItem("Back", &navigatorMenuChangeStatic, 0);
    for(int i=0; i<this->joystick->getAxisCount(); i++){
        settingsMenuAxes->addItem(("Axis " + String(i)), &MyJoystick::axisEntryStatic, i);
        this->joystick->axis[i]->initSettingsMenu(this);
    }


    settingsMenuButtons->addItem("Back", &navigatorMenuChangeStatic, 0);
    for(int i=0; i<this->joystick->getButtonCount(); i++){
        //settingsMenuButtons->addItem(("Button " + String(i)), &MyJoystick::buttonEntryStatic, i);
    }




    testMenu->addItem("Back", &navigatorMenuChangeStatic, 0);

    testMenu->addItem("Hallo Navigator1");
    testMenu->addItem("Hallo Navigator2");
    testMenu->addItem("Hallo Navigator3");
    testMenu->addItem("Hallo Navigator4");
    testMenu->addItem("Hallo Navigator5");
    testMenu->addItem("Hallo Navigator6");
    testMenu->addItem("Hallo Navigator7");
}

void Navigator::init(Encoder *encoder) {
    this->encoder = encoder;
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
    if(encoder->mode != 0){
        encoder->mode = 0;
    }else{
        menuHistoryPointer--;
        display->currentCursorPos= menuHistoryCursorPos[menuHistoryPointer];
        display->scrollOffset = menuHistoryScrollOffset[menuHistoryPointer];
    }
    this->display->setNewMenu(menuHistory[menuHistoryPointer], true);


}

void Navigator::checkEncoderFlag() {
    if (encoder->flag != 0) {

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
            case 3:
                break;
            case 100:
                break;
        }
        encoder->resetEncoderFlag();
        delay(10);
    }

}

void Navigator::checkEncoderButtonFlag() {
    if (encoder->buttonFlag) {
        switch (encoder->mode) {
            case 0:
                display->currentMenu->executeFunction(display->currentCursorPos + display->scrollOffset);
                break;
            case 1:
                //input->setValue();
                break;
            case 2:
                //input->setSelect();
                break;
            case 3:
                //input->returnToMenu();
                break;
            case 100:
                break;
        }
        encoder->buttonFlag = false;
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


void Navigator::itemAction(uint8_t index){
    String text[] = {"Hallo", "1", "true", "false", "false2"};
    String text2[] = {"Hallo", "SplashScreen", "Hier", "Wow"};

    uint8_t buttonDemuxPins[] = {BUTTON_DEMUX_E, BUTTON_DEMUX_S0, BUTTON_DEMUX_S1, BUTTON_DEMUX_S2};
    uint8_t buttonColumnPins[] = {BUTTON_COLUMN_0, BUTTON_COLUMN_1, BUTTON_COLUMN_2};

    switch (index) {
        case 0:
            joystick->toggleOutput();
            break;
        case 1:
            input->showEEPROMWaitScreen();
            joystick->storeGeneralConfig();
            joystick->storePreset();
            input->endWaitScreen();
            break;
        case 2:
            input->resetPresetPrompt();
            itemAction(199);
            break;
        case 3:
            input->factoryResetPrompt();
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
    }

}




void Navigator::navigatorMenuChangeStatic(Navigator *navigator, uint8_t index) {
    navigator->menuChange(index);
}

void Navigator::navigatorItemActionStatic(Navigator *navigator, uint8_t index) {
    navigator->itemAction(index);

}

