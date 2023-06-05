// Copyright (c) 2023 Leonhard Baschang

#ifndef INC_24_LIB_EEPROM_MICROCHIP_24_H
#define INC_24_LIB_EEPROM_MICROCHIP_24_H

#include "Wire.h"
#define PAGE_WRITE_MAX_DEFAULT 32


class EEPROM_Microchip_24 {
#if defined(ARDUINO_RASPBERRY_PI_PICO)
    MbedI2C *i2c;
#else
    TwoWire *i2c;
#endif


    uint8_t i2cAddress;
    uint16_t highestAddress;
    uint8_t maxPageWrite;
    uint8_t *buffer = nullptr;

    bool setAddress(uint16_t address);
    void waitUntilACKSend();

public:

    EEPROM_Microchip_24(uint8_t i2cAddress, uint16_t highestAddress, uint8_t sdaPin = PIN_WIRE_SDA,
                        uint8_t sclPin = PIN_WIRE_SCL, uint8_t maxPageWrite = PAGE_WRITE_MAX_DEFAULT);


    EEPROM_Microchip_24(const bool addressBits[3], uint16_t highestAddress,
                        uint8_t sdaPin = PIN_WIRE_SDA, uint8_t sclPin = PIN_WIRE_SCL,
                        uint8_t maxPageWrite = PAGE_WRITE_MAX_DEFAULT);


    bool writeByte(uint16_t address, uint8_t data);

    bool writePage(uint16_t startAddress, uint8_t *data, uint8_t dataLength);

    uint8_t readByte(uint16_t address);

    uint8_t *readSequentialByte(uint16_t startAddress, uint8_t length);

    bool updateByte(uint16_t address, uint8_t data);

    bool updatePage(uint16_t startAddress, uint8_t *data, uint8_t length);
};


#endif //INC_24_LIB_EEPROM_MICROCHIP_24_H
