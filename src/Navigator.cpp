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


Navigator::Navigator(Display *display, MyJoystick *joystick) {
    this->display = display;
    this->joystick = joystick;
    input = new Input(this);

    testMenu = new Menu(this);
    mainMenu = new Menu(this);
    settingsMenu = new Menu(this);

    settingsMenuGeneral = new Menu(this);
    settingsMenuAxes = new Menu(this);
    settingsMenuButtons = new Menu(this);

    mainMenu->addItem("Porsche Muehk");
    mainMenu->addItem("Settings", &navigatorMenuChangeStatic, 2);
    mainMenu->addItem("Debug", &navigatorMenuChangeStatic, 3);
    //mainMenu->addItem("Test", &navigatorItemActionStatic, 200);
    //mainMenu->addItem("Test2", &navigatorItemActionStatic, 201);
    //mainMenu->addItem("Test3", &navigatorItemActionStatic, 202);

    settingsMenu->addItem("Back", &navigatorMenuChangeStatic, 0);
    settingsMenu->addItem("General", &navigatorMenuChangeStatic, 20);
    settingsMenu->addItem("Axes", &navigatorMenuChangeStatic, 21);
    settingsMenu->addItem("Buttons", &navigatorMenuChangeStatic, 22);

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
            setNextMenu(settingsMenuGeneral);
            break;
        case 21:
            setNextMenu(settingsMenuAxes);
            break;
        case 22:
            setNextMenu(settingsMenuButtons);
            break;
        default:
            break;

    }
}

void Navigator::itemAction(uint8_t index){
    String text[] = {"Hallo", "1", "true", "false", "false2"};
    String text2[] = {"Hallo", "SplashScreen", "Hier", "Wow"};
    switch (index) {
        case 200:
            input->valueInput("Test Value Input", &testValue, testValue, 2.0, -1.0, 0.1, 2);
            break;
        case 201:

            input->selectInput("Test Select Input", &testSelectInput, testSelectInput, text, 5);
            break;

        case 202:

            input->setSplashScreen(text2);
            break;
    }

}




void Navigator::navigatorMenuChangeStatic(Navigator *navigator, uint8_t index) {
    navigator->menuChange(index);
}

void Navigator::navigatorItemActionStatic(Navigator *navigator, uint8_t index) {
    navigator->itemAction(index);

}

