
// Copyright (c) 2023 Leonhard Baschang

#include "Arduino.h"
#include "Wire.h"
#include "EEPROM_Microchip_24.h"


EEPROM_Microchip_24::EEPROM_Microchip_24(uint8_t i2cAddress, uint16_t highestAddress, uint8_t sdaPin, uint8_t sclPin,
                                         uint8_t maxPageWrite) {
    this->i2cAddress = i2cAddress;
    this->highestAddress = highestAddress;
    this->maxPageWrite = maxPageWrite;

#if defined(ARDUINO_RASPBERRY_PI_PICO)
    i2c = new MbedI2C(sdaPin, sclPin);
#else
    i2c = new TwoWire();
#endif

    i2c->begin();

}


EEPROM_Microchip_24::EEPROM_Microchip_24(const bool addressBits[3], uint16_t highestAddress,
                                         uint8_t sdaPin,
                                         uint8_t sclPin, uint8_t maxPageWrite) {
    this->i2cAddress = 0b1010 << 3 | (addressBits[2] & 0x01) << 2 | (addressBits[1] & 0x01) << 1 | (addressBits[0] & 0x01);
    this->highestAddress = highestAddress;
    this->maxPageWrite = maxPageWrite;

#if defined(ARDUINO_RASPBERRY_PI_PICO)
    i2c = new MbedI2C(sdaPin, sclPin);
#else
    i2c = new TwoWire();
#endif
    i2c->begin();

}


bool EEPROM_Microchip_24::setAddress(uint16_t address) {
    if (address > highestAddress) {
        return false;
    }
    i2c->beginTransmission(i2cAddress);

    if (highestAddress > 0xFF) {

        i2c->write((address >> 8) & 0xFF);
        i2c->write(address & 0xFF);
    } else {
        i2c->write(address & 0xFF);
    }

    return true;
}


bool EEPROM_Microchip_24::writeByte(uint16_t address, uint8_t data) {
    if (!setAddress(address)) {
        return false;
    }
    i2c->write(data);
    waitUntilACKSend();
    return true;
}

bool EEPROM_Microchip_24::writePage(uint16_t startAddress, uint8_t *data, uint8_t dataLength) {
    if (!setAddress(startAddress)) {
        return false;
    }
    for (int i = 0; i < dataLength; i++) {
        i2c->write(data[i]);
    }
    waitUntilACKSend();
    return true;
}


uint8_t EEPROM_Microchip_24::readByte(uint16_t address) {

    if (!setAddress(address)) {
        return NULL;
    }
    waitUntilACKSend();
    i2c->requestFrom(i2cAddress, 1);
    uint8_t data = i2c->read();
    return data;
}

uint8_t *EEPROM_Microchip_24::readSequentialByte(uint16_t startAddress, uint8_t length) {
    if (!setAddress(startAddress)) {
        return nullptr;
    }
    waitUntilACKSend();
    if (length > maxPageWrite || length > 32) {
        return nullptr;
    }

    buffer = new uint8_t[length];
    uint8_t index = 0;

    i2c->requestFrom(i2cAddress, length);
    while (i2c->available()) {
        buffer[index] = i2c->read();
        index++;
    }

    return buffer;
}

bool EEPROM_Microchip_24::updateByte(uint16_t address, uint8_t data) {
    uint8_t currentData = readByte(address);
    if (currentData != data) {
        writeByte(address, data);
        return true;
    } else {
        return false;
    }

}


bool EEPROM_Microchip_24::updatePage(uint16_t startAddress, uint8_t *data, uint8_t length) {
    bool changed = false;
    for (int i = 0; i < length; i++) {
        changed = updateByte(startAddress + i, data[i]);
    }

    return changed;

}

void EEPROM_Microchip_24::waitUntilACKSend() {
    uint8_t i2cReturn = 0;
    do{
        i2cReturn = i2c->endTransmission();
    }while(i2cReturn == 2 || i2cReturn == 3);

}






