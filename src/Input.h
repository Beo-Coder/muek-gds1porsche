//
// Created by Leo on 11.04.2023.
//

#ifndef GDS1PORSCHEMUEHK_INPUT_H
#define GDS1PORSCHEMUEHK_INPUT_H

#include "Axis.h"


class Navigator;

class Input {

    Navigator *navigator;

    double currentValue;
    double previousValue;
    double *valuePointer;
    double maxValue;
    double minValue;
    double stepSize;
    uint8_t displayResolution;

    uint8_t *selectValuePointer;
    uint8_t selectIndex;
    uint8_t previousSelectIndex;
    String selectValues[10];
    uint8_t selectValuesSize;
    uint8_t scrollOffset = 0;





    void displayValue();

    void displaySelectInput();
    void displaySelectCursor();
    void displaySplashScreen(String *values);
    void waitUntilButtonPressed();

public:


    Input(Navigator *navigator);



    void valueInput(const String &header, double *value, double startValue, double pMaxValue, double pMinValue,
                    double pStepSize, uint8_t pDisplayResolution);

    void changeValueInput(int8_t direction);


    void setValue();


    void selectInput(const String &header, uint8_t *value, uint8_t startIndex, String *values, uint8_t valuesSize);



    void changeSelectInput(int8_t direction);


    void setSelect();

    void setSplashScreen(String *values);

    void returnToMenu();





    void axisCalibration(Axis *axis);


    void axisCenterCalibrationPrompt(Axis *axis);
    void axisSetBase(Axis *axis);

    void axisSetMode(Axis *axis);

    void axisCalibrationDigital(Axis *axis);

    void generalSetAxisCount();
    void generalSetButtonCount();
    void generalSetMode();

    void reinitPrompt();

    void showWaitScreen(String *values);
    void endWaitScreen();

    void showEEPROMWaitScreen();

    void resetPresetPrompt();
    void factoryResetPrompt();
};


#endif //GDS1PORSCHEMUEHK_INPUT_H
