// Copyright (c) 2023. Leonhard Baschang
#include <Arduino.h>
#include "Display.h"
#include "Menu.h"
#include "Navigator.h"
#include "Encoder.h"
#include "MyJoystick.h"
#include "EEPROM_Microchip_24.h"
#include "MCP3204_MCP3208.h"
#include "header.h"


#include "LiquidCrystal_I2C.h"

#include "Axis.h"





uint8_t buttonDemuxPins[] = {BUTTON_DEMUX_E, BUTTON_DEMUX_S0, BUTTON_DEMUX_S1, BUTTON_DEMUX_S2};
uint8_t buttonColumnPins[] = {BUTTON_COLUMN_0, BUTTON_COLUMN_1, BUTTON_COLUMN_2};





unsigned long lastEncoderButtonPress = 100;
bool lastButtonState = false;

unsigned long lastEncoderTime = 100;










EEPROM_Microchip_24 eeprom(EEPROM_ADDRESS, EEPROM_MAX_MEMORY_ADDRESS, EEPROM_SDA_PIN, EEPROM_SCL_PIN);

MCP3204_MCP3208 adc(AD_CHANNEL_COUNT, AD_SPI_CS, AD_SPI_SCK, AD_SPI_MOSI, AD_SPI_MISO);

Display display(0x27, LCD_WIDTH, LCD_HEIGHT);

MyJoystick joystick(&adc, &eeprom, buttonDemuxPins, buttonColumnPins);

Navigator navigator(&display, &joystick);

//KeyboardHID keyboard;

void encoderISR() {


    if (digitalRead(ENCODER_DT_PIN)) {
        navigator.encoder->setEncoderFlag(1);

    } else {
        navigator.encoder->setEncoderFlag(-1);
        //digitalWrite(25, LOW);
    }




}
void encoderButtonISR() {
    lastButtonState = !lastButtonState;
    unsigned long currentMillis = millis();
    if((currentMillis - lastEncoderButtonPress) > ENCODER_BUTTON_DEBOUNCE_TIME){
        navigator.encoder->setButtonFlag();
        lastEncoderButtonPress = currentMillis;
    }

}

Encoder encoder(ENCODER_CLK_PIN, ENCODER_DT_PIN, ENCODER_BUTTON_PIN, &encoderISR, &encoderButtonISR);



void setup() {

    Serial1.begin(9600);



    pinMode(25, OUTPUT);



    Serial1.println("---------------------------------------");
    Serial1.println("---------------------------------------");
    Serial1.println("---------------------------------------");
    Serial1.println("Begin");


    display.init();
    navigator.init(&encoder);
    joystick.begin();









}


void loop() {




    //display.lcd->setCursor(0,0);
    //display.lcd->print(double(random(0,1000000))/10000,16);




    joystick.updateAxis();
    joystick.updateButton();




    navigator.checkEncoderFlag();

    navigator.encoder->checkButton();
    navigator.checkEncoderButtonFlag();






}

