//
// Created by Leo on 11.04.2023.
//

#include "Arduino.h"
#include "Encoder.h"

Encoder::Encoder(uint8_t clkPin, uint8_t dtPin, uint8_t buttonPin, void (*isr)()) {
    pinMode(clkPin, INPUT);
    pinMode(dtPin, INPUT);
    pinMode(buttonPin, INPUT);
    this->buttonPin = buttonPin;

    attachInterrupt(digitalPinToInterrupt(clkPin), isr, FALLING);


}

void Encoder::setEncoderFlag(int8_t direction) {
    flag = direction;
}

void Encoder::resetEncoderFlag() {
    flag = 0;
}

void Encoder::setButtonFlag() {
    buttonFlag = true;
}

bool Encoder::readButton() const {
    return digitalRead(buttonPin);
}

void Encoder::checkButton() {
    uint8_t buttonState = readButton();
    if (buttonState && !previousButtonState) {
        previousButtonState = true;
        setButtonFlag();
    } else if (!buttonState && previousButtonState) {
        previousButtonState = false;
    }

}
