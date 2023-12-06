// Copyright (c) 2023. Leonhard Baschang
#include <Arduino.h>
#include "Display.h"
#include "Navigator.h"
#include "Encoder.h"
#include "MyJoystick.h"
#include "EEPROM_Microchip_24.h"
#include "MCP3204_MCP3208.h"
#include "header.h"
#include "LiquidCrystal_I2C.h"


uint8_t buttonDemuxPins[] = {BUTTON_DEMUX_E, BUTTON_DEMUX_S0, BUTTON_DEMUX_S1, BUTTON_DEMUX_S2};
uint8_t buttonColumnPins[] = {BUTTON_COLUMN_0, BUTTON_COLUMN_1, BUTTON_COLUMN_2};


EEPROM_Microchip_24 eeprom(EEPROM_ADDRESS, EEPROM_MAX_MEMORY_ADDRESS, EEPROM_SDA_PIN, EEPROM_SCL_PIN);

MCP3204_MCP3208 adc(AD_CHANNEL_COUNT, AD_SPI_CS, AD_SPI_SCK, AD_SPI_MOSI, AD_SPI_MISO);

Display display(0x27, LCD_WIDTH, LCD_HEIGHT);

MyJoystick joystick(&adc, &eeprom, buttonDemuxPins, buttonColumnPins);

Navigator navigator(&display, &joystick);


void encoderISR() {


    if (digitalRead(ENCODER_DT_PIN)) {
        navigator.encoder->setEncoderFlag(-1);

    } else {
        navigator.encoder->setEncoderFlag(1);

    }


}

Encoder encoder(ENCODER_CLK_PIN, ENCODER_DT_PIN, ENCODER_BUTTON_PIN, &encoderISR);


void setup() {

    Serial1.begin(9600);


    pinMode(25, OUTPUT);


    Serial1.println("---------------------------------------");
    Serial1.println("---------------------------------------");
    Serial1.println("---------------------------------------");
    Serial1.println("Porsche Muehk V" + String(VERSION));
    Serial1.println("Made by Leonhard Baschang ( ._.)");
    Serial1.println("J1.2 2022/23");
    Serial1.println("J2.2 2023/24");
    Serial1.println("Made for GDS1");
    Serial1.println("---------------------------------------");


    display.init();
    navigator.init(&encoder);
    joystick.begin();


}


void loop() {


    joystick.updateAxis();
    joystick.updateButton();


    navigator.checkEncoderFlag();

    navigator.encoder->checkButton();
    navigator.checkEncoderButtonFlag();
    navigator.checkDisplay();


}

