//
// Created by Leo on 11.04.2023.
//

#ifndef GDS1PORSCHEMUEHK_ENCODER_H
#define GDS1PORSCHEMUEHK_ENCODER_H


class Encoder {

    bool readButton() const;
    bool previousButtonState{};


public:
    uint8_t buttonPin = 0;

    uint8_t mode = 0;
    int8_t flag = 0;
    bool buttonFlag = false;

    Encoder(uint8_t clkPin, uint8_t dtPin, uint8_t buttonPin, void (*isr)());

    void setEncoderFlag(int8_t direction);

    void setButtonFlag();



    void checkButton();

    void resetEncoderFlag();

};


#endif //GDS1PORSCHEMUEHK_ENCODER_H
