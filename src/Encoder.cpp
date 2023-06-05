//
// Created by Leo on 11.04.2023.
//

#include "Arduino.h"
#include "Encoder.h"

Encoder::Encoder(uint8_t clkPin, uint8_t dtPin, uint8_t buttonPin, void (*isr)(), void (*buttonIsr)()) {
    pinMode(clkPin, INPUT);
    pinMode(dtPin, INPUT);
    pinMode(buttonPin, INPUT);
    this->buttonPin = buttonPin;
    this->isr = isr;

    attachInterrupt(digitalPinToInterrupt(clkPin), isr, FALLING);
    //attachInterrupt(buttonPin, buttonIsr, FALLING);


}

void Encoder::setEncoderFlag(int8_t direction) {
    flag = direction;
}

void Encoder::resetEncoderFlag(){
    flag = 0;
}

void Encoder::setButtonFlag() {
    buttonFlag = true;
}

bool Encoder::readButton() const{
    return digitalRead(buttonPin);
}
void Encoder::checkButton() {
    if (readButton() && !previousButtonState) {
        previousButtonState = true;
        setButtonFlag();
    } else if (!readButton() && previousButtonState) {
        previousButtonState = false;
    }

}
