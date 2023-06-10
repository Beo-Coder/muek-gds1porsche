//
// Created by Leo on 11.04.2023.
//

#include "Arduino.h"
#include "Input.h"
#include "Navigator.h"
#include "LiquidCrystal_I2C.h"
#include "Encoder.h"
#include "Axis.h"
#include "MyJoystick.h"


Input::Input(Navigator *navigator) {
    this->navigator = navigator;

}


void Input::valueInput(const String &header, double *value, double startValue, double pMaxValue, double pMinValue,
                       double pStepSize, uint8_t pDisplayResolution) {
    this->navigator->encoder->mode = 1;


    this->valuePointer = value;
    this->currentValue = startValue;
    this->maxValue = pMaxValue;
    this->minValue = pMinValue;
    this->stepSize = pStepSize;
    this->displayResolution = pDisplayResolution;


    navigator->display->lcd->clear();
    navigator->display->lcd->setCursor(0, 0);
    navigator->display->lcd->print(header);

    displayValue();
}

bool areDoubleSame(double dFirstVal, double dSecondVal) {
    return std::fabs(dFirstVal - dSecondVal) < 1E-5;
}

void Input::changeValueInput(int8_t direction) {
    previousValue = currentValue;

    if (direction == 1) {
        currentValue = currentValue + stepSize;
    } else {
        currentValue = currentValue - stepSize;
    }


    if (areDoubleSame(currentValue, maxValue + stepSize)) {
        currentValue = currentValue - stepSize;
    } else if (areDoubleSame(currentValue, minValue - stepSize)) {
        currentValue = currentValue + stepSize;
    }


    displayValue();

}

void Input::setValue() {
    *valuePointer = currentValue;
}

void Input::displayValue() {

    uint8_t previousSize = String(previousValue).length();
    uint8_t currentSize = String(currentValue).length();
    if (previousSize != currentSize) {
        navigator->display->lcd->setCursor(0, 1);
        navigator->display->lcd->print(navigator->display->blankLine);
    }


    navigator->display->lcd->setCursor(0, 1);
    if (displayResolution == 0) {
        navigator->display->lcd->print(int(currentValue));
    } else {
        navigator->display->lcd->print(currentValue, displayResolution);
    }


}

void
Input::selectInput(const String &header, uint8_t *value, uint8_t startIndex, String *values, uint8_t valuesSize) {
    this->navigator->encoder->mode = 2;


    this->selectValuePointer = value;
    this->selectIndex = startIndex;
    for (int i = 0; i < valuesSize; i++) {
        selectValues[i] = values[i];
    }

    this->selectValuesSize = valuesSize;


    navigator->display->lcd->clear();
    navigator->display->lcd->setCursor(1, 0);
    navigator->display->lcd->print(header);

    if (this->selectIndex - (navigator->display->lcdHeight - 1) >= 0) {
        scrollOffset = this->selectIndex - (navigator->display->lcdHeight - 1) + 1;
    } else {
        scrollOffset = 0;
    }


    displaySelectInput();
    displaySelectCursor();
}

void Input::displaySelectInput() {

    uint8_t maxI;
    if (selectValuesSize < (navigator->display->lcdHeight - 1)) {
        maxI = selectValuesSize;
    } else {
        maxI = (navigator->display->lcdHeight - 1);
    }

    for (int i = 0; i < maxI; i++) {
        navigator->display->lcd->setCursor(1, i + 1);
        navigator->display->lcd->print(navigator->display->blankLine);

        navigator->display->lcd->setCursor(1, i + 1);
        navigator->display->lcd->print(selectValues[i + scrollOffset]);

    }
    if (selectValuesSize - (navigator->display->lcdHeight - 1) > 0 && scrollOffset != 0) {
        navigator->display->lcd->setCursor(navigator->display->lcdWidth - 1, 0);
        navigator->display->lcd->write(0);
    } else {
        navigator->display->lcd->setCursor(navigator->display->lcdWidth - 1, 0);
        navigator->display->lcd->print(" ");
    }

    if (selectValuesSize - (navigator->display->lcdHeight - 1) - scrollOffset > 0) {
        navigator->display->lcd->setCursor(navigator->display->lcdWidth - 1, navigator->display->lcdHeight - 1);
        navigator->display->lcd->write(1);
    } else {
        navigator->display->lcd->setCursor(navigator->display->lcdWidth - 1, navigator->display->lcdHeight - 1);
        navigator->display->lcd->print(" ");
    }


}

void Input::displaySelectCursor() {
    navigator->display->lcd->setCursor(0, previousSelectIndex - scrollOffset + 1);
    navigator->display->lcd->print(" ");
    navigator->display->lcd->setCursor(0, selectIndex - scrollOffset + 1);
    navigator->display->lcd->print(">");


}

void Input::changeSelectInput(int8_t direction) {
    previousSelectIndex = selectIndex;
    if (direction == 1) {
        if (selectIndex < selectValuesSize - 1) {
            selectIndex = selectIndex + 1;

            if (selectIndex - scrollOffset >= navigator->display->lcdHeight - 1) {
                scrollOffset++;
                displaySelectInput();
            }
        }

    } else {
        if (selectIndex > 0) {
            selectIndex = selectIndex - 1;
            if (selectIndex - scrollOffset < 0) {
                scrollOffset--;
                displaySelectInput();

            }
        }
    }

    displaySelectCursor();
}

void Input::setSelect() {
    *selectValuePointer = selectIndex;
}


void Input::setSplashScreen(String *values) {
    this->navigator->encoder->mode = 3;
    displaySplashScreen(values);


}

void Input::displaySplashScreen(String *values) {
    navigator->display->lcd->clear();
    for (int i = 0; i < navigator->display->lcdHeight; i++) {
        navigator->display->lcd->setCursor(0, i);
        navigator->display->lcd->print(values[i]);
    }

}

void Input::returnToMenu() {
    navigator->setPreviousMenu();
}


void Input::waitUntilButtonPressed() {
    navigator->encoder->buttonFlag = false;
    while (!navigator->encoder->buttonFlag) {
        navigator->encoder->checkButton();
        navigator->checkEncoderFlag();
    }
    navigator->encoder->buttonFlag = false;
}

void Input::axisCalibration(Axis *axis) {
    navigator->encoder->mode = 100;
    navigator->joystick->disableOutput();
    uint16_t data[3];

    String screenText[] = {"Set axis " + String(axis->getIndex()) + " to", "minimum position", "Confirm with button",
                           ""};
    displaySplashScreen(screenText);
    waitUntilButtonPressed();
    data[0] = axis->readSensor();

    screenText[1] = "maximum position";
    displaySplashScreen(screenText);
    waitUntilButtonPressed();
    data[1] = axis->readSensor();

    //ask for center cal

    uint8_t value = 0;
    if (axis->getCalibrateCenter()) {
        value = 0;
    } else {
        value = 1;
    }
    String textValues[] = {"Yes", "No"};
    selectInput("Center cal. axis " + String(axis->getIndex()), &value, value, textValues, 2);
    waitUntilButtonPressed();
    setSelect();
    if (value == 0) {
        axis->setCalibrateCenter(true);
        screenText[1] = "center position";
        displaySplashScreen(screenText);
        waitUntilButtonPressed();
        data[2] = axis->readSensor();
    } else {
        axis->setCalibrateCenter(false);
        uint16_t smaller;
        uint16_t bigger;
        if (data[0] < data[1]) {
            smaller = data[0];
            bigger = data[1];

        } else {
            smaller = data[1];
            bigger = data[0];
        }
        data[2] = ((bigger - smaller) / 2) + smaller;
    }

    axis->setCalibrationData(data);


    navigator->joystick->enableOutput();
    returnToMenu();

}


void Input::axisSetBase(Axis *axis) {
    navigator->encoder->mode = 100;
    if (axis->getMode() == 0) {
        String screenText[] = {"Wrong mode", "Please set to expo.", "or log. mode", ""};
        displaySplashScreen(screenText);
        waitUntilButtonPressed();


    } else {
        navigator->joystick->disableOutput();
        uint8_t value = 0;
        String textValues[] = {"Different bases", "Same bases", "Reset base"};
        selectInput("Center cal. axis " + String(axis->getIndex()), &value, value, textValues, 3);
        waitUntilButtonPressed();
        setSelect();

        double base[2];
        double maxBase;
        double minBase;

        if (axis->getMode() == 1) {
            maxBase = BASE_MAX_EXPO;
            minBase = BASE_MIN_EXPO;
        } else {
            maxBase = BASE_MAX_LOG;
            minBase = BASE_MIN_LOG;
        }

        double *currentBase = axis->getBase();
        if (value == 0) {
            valueInput("Set base 0", &base[0], currentBase[0], maxBase, minBase, BASE_STEP_SIZE,
                       BASE_DISPLAY_RESOLUTION);
            waitUntilButtonPressed();
            setValue();

            valueInput("Set base 1", &base[1], currentBase[1], maxBase, minBase, BASE_STEP_SIZE,
                       BASE_DISPLAY_RESOLUTION);
            waitUntilButtonPressed();
            setValue();


            axis->setBase(base[0], base[1]);
        } else if (value == 1) {
            valueInput("Set bases", &base[0], currentBase[0], maxBase, minBase, BASE_STEP_SIZE,
                       BASE_DISPLAY_RESOLUTION);
            waitUntilButtonPressed();
            setValue();

            axis->setBase(base[0], base[0]);

        } else {
            axis->setBase(FACTORY_AXIS_BASE, FACTORY_AXIS_BASE);
        }
        navigator->joystick->enableOutput();

    }
    returnToMenu();

}

void Input::axisSetMode(Axis *axis) {
    navigator->encoder->mode = 100;
    uint8_t value = axis->getMode();

    String textValues[] = {"Linear", "Exponential", "Logarithm", "Digital"};
    selectInput("Set mode for axis " + String(axis->getIndex()), &value, value, textValues, 4);
    waitUntilButtonPressed();
    setSelect();

    axis->setMode(value);

    returnToMenu();

}

void Input::axisCalibrationDigital(Axis *axis) {
    navigator->encoder->mode = 100;
    navigator->joystick->disableOutput();

    uint16_t data[2];

    String screenText[] = {"Set axis " + String(axis->getIndex()) + " to", "lower border", "Confirm with button", ""};
    displaySplashScreen(screenText);
    waitUntilButtonPressed();
    data[0] = axis->readSensor();

    screenText[1] = "upper border";
    displaySplashScreen(screenText);
    waitUntilButtonPressed();
    data[1] = axis->readSensor();

    axis->setDigitalCalibrationData(data);


    navigator->joystick->enableOutput();
    returnToMenu();

}

void Input::generalSetAxisCount() {
    navigator->encoder->mode = 100;
    navigator->joystick->disableOutput();
    double value;
    valueInput("Set axis count", &value, navigator->joystick->getNewAxisCount(), MAX_AXIS_COUNT, 0, 1,
               0);
    waitUntilButtonPressed();
    setValue();
    navigator->joystick->setNewAxisCount(uint8_t(value));

    reinitPrompt();
    returnToMenu();


}

void Input::generalSetButtonCount() {
    navigator->encoder->mode = 100;
    navigator->joystick->disableOutput();
    double value;
    valueInput("Set button count", &value, navigator->joystick->getNewButtonCount(), MAX_BUTTON_COUNT, 0, 1,
               0);
    waitUntilButtonPressed();
    setValue();
    navigator->joystick->setNewButtonCount(uint8_t(value));

    reinitPrompt();
    returnToMenu();


}

void Input::generalSetMode() {
    navigator->encoder->mode = 100;
    navigator->joystick->disableOutput();
    uint8_t value = navigator->joystick->getNewJoystickMode();

    String textValues[] = {"Analog", "Analog/Digital", "Keyboard"};
    selectInput("Digital axes mode", &value, value, textValues, 3);
    waitUntilButtonPressed();
    setSelect();

    navigator->joystick->setNewJoystickMode(value);
    reinitPrompt();


    returnToMenu();


}

void Input::generalSetPreset() {
    navigator->encoder->mode = 100;
    uint8_t value = navigator->joystick->getPreset();
    String textValues[EEPROM_PRESET_COUNT];
    for (int i = 0; i < EEPROM_PRESET_COUNT; i++) {
        textValues[i] = "Preset " + String(i);
    }
    selectInput("Current preset: " + String(value), &value, value, textValues, EEPROM_PRESET_COUNT);
    waitUntilButtonPressed();
    setSelect();
    navigator->joystick->setNewPreset(value);

    if (navigator->joystick->getPreset() != value) {
        showEEPROMWaitScreen();
        navigator->joystick->storeGeneralConfig();
        endWaitScreen();
    }
    returnToMenu();

}

void Input::generalSavePreset() {
    navigator->encoder->mode = 100;
    uint8_t value = navigator->joystick->getPreset();
    String textValues[EEPROM_PRESET_COUNT];
    for (int i = 0; i < EEPROM_PRESET_COUNT; i++) {
        textValues[i] = "Preset " + String(i);
    }
    selectInput("Save config to", &value, value, textValues, EEPROM_PRESET_COUNT);
    waitUntilButtonPressed();
    setSelect();

    showEEPROMWaitScreen();
    navigator->joystick->storePreset(value);
    endWaitScreen();

}

void Input::reinitPrompt() {
    navigator->encoder->mode = 100;
    String screenText[] = {"Please save settings", "and reinit joystick", "", ""};
    displaySplashScreen(screenText);
    waitUntilButtonPressed();

}

void Input::showWaitScreen(arduino::String *values) {
    navigator->encoder->mode = 100;
    navigator->display->lcd->clear();
    for (int i = 0; i < navigator->display->lcdHeight; i++) {
        navigator->display->lcd->setCursor(0, i);
        navigator->display->lcd->print(values[i]);
    }
}

void Input::endWaitScreen() {
    navigator->display->lcd->clear();
    returnToMenu();
}

void Input::showEEPROMWaitScreen() {
    String eepromWriteText[] = {"Data is stored", "Please wait.", "Do not unplug!", ""};
    showWaitScreen(eepromWriteText);
}

void Input::resetPresetPrompt() {
    navigator->encoder->mode = 100;
    uint8_t value;
    String textValues[] = {"No", "Yes"};
    selectInput("Reset preset " + String(navigator->joystick->getPreset()) + "?", &value, 0, textValues, 2);
    waitUntilButtonPressed();
    setSelect();

    if (value == 1) {
        showEEPROMWaitScreen();
        navigator->joystick->resetPreset(navigator->joystick->getPreset());
        endWaitScreen();
    }

    returnToMenu();
}

void Input::factoryResetPrompt() {
    navigator->encoder->mode = 100;
    uint8_t value;
    String textValues[] = {"No", "Yes"};
    selectInput("Reset ALL settings?", &value, 0, textValues, 2);
    waitUntilButtonPressed();
    setSelect();
    if (value == 0) {
        returnToMenu();
        return;
    }
    selectInput("Really?!", &value, 0, textValues, 2);
    waitUntilButtonPressed();
    setSelect();

    if (value == 1) {
        showEEPROMWaitScreen();
        navigator->joystick->factoryReset();
        endWaitScreen();
    }

    returnToMenu();
}


void Input::buttonMode(Button *button) {
    navigator->encoder->mode = 100;
    navigator->joystick->disableOutput();
    uint8_t value = button->getTogglMode();
    if (value) {
        value = 1;
    } else {
        value = 0;
    }

    String textValues[] = {"Button", "Toggle"};
    selectInput("Button Mode", &value, value, textValues, 2);
    waitUntilButtonPressed();
    setSelect();
    if (value == 0) {
        button->setToggleMode(false);
    } else {
        button->setToggleMode(true);
    }
    navigator->joystick->enableOutput();
    returnToMenu();

}

void Input::buttonType(Button *button) {
    navigator->encoder->mode = 100;
    navigator->joystick->disableOutput();
    uint8_t value = button->getNormalOpen();
    if (value) {
        value = 0;
    } else {
        value = 1;
    }

    String textValues[] = {"Normally Open", "Normally Closed"};
    selectInput("Button Type", &value, value, textValues, 2);
    waitUntilButtonPressed();
    setSelect();
    if (value == 0) {
        button->setNormalOpen(true);
    } else {
        button->setNormalOpen(false);
    }
    navigator->joystick->enableOutput();

    returnToMenu();

}

