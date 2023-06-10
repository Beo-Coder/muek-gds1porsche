// Copyright (c) 2023. Leonhard Baschang

#include "Arduino.h"
#include "PluggableUSBHID.h"
#include "KeyboardHID.h"

#define JOYSTICK_AXIS_MINIMUM 0
#define JOYSTICK_AXIS_MAXIMUM 65535

using namespace arduino;


KeyboardHID::KeyboardHID() {

    _asciimap = KEYBOARDLAYOUT_DE;
    asciimapSize = sizeof(KEYBOARDLAYOUT_DE) / sizeof(KEYBOARDLAYOUT_DE[0]);


    releaseAll();





    //sendState();


}

uint16_t KeyboardHID::getKeycode(uint16_t k) {
    // Ignore invalid input
    if (k >= asciimapSize) {
        return 0;
    }

    // Read key from ascii lookup table
    uint16_t key = pgm_read_word(_asciimap + k);

    return key;
}


void KeyboardHID::press(uint16_t k, bool isKeyCode) {

    uint16_t keyCode;
    if (!isKeyCode) {
        keyCode = getKeycode(k);
    } else {
        keyCode = k;
    }

    uint8_t modifier = ((keyCode >> 8) & 0xFF);
    if (modifier != 0) {
        _keyReport.modifiers |= modifier;
    }

    _keyReport.keys[(keyCode & 0xFF) >> 3] |= 1 << ((keyCode & 0xFF) & 0x7);
    sendState();
}

void KeyboardHID::release(uint16_t k, bool isKeyCode) {
    uint16_t keyCode;
    if (!isKeyCode) {
        keyCode = getKeycode(k);
    } else {
        keyCode = k;
    }
    uint8_t modifier = ((keyCode >> 8) & 0xFF);
    if (modifier != 0) {
        _keyReport.modifiers &= ~modifier;
    }
    _keyReport.keys[(keyCode & 0xFF) >> 3] &= ~(1 << ((keyCode & 0xFF) & 0x7));
    sendState();
}

void KeyboardHID::releaseAll() {
    _keyReport.modifiers = 0;
    _keyReport.reserved = 0;
    for (unsigned char &key: _keyReport.keys) {
        key = 0;
    }
    sendState();
}

void KeyboardHID::pressAndRelease(uint16_t k, bool isKeyCode) {
    press(k, isKeyCode);    // Keydown
    release(k, isKeyCode); // Keyup

}


bool KeyboardHID::sendState() {
    _mutex.lock();


    HID_REPORT report;
    report.data[0] = DEFAULT_REPORT_ID_KEYBOARD;
    uint8_t index = 1;

    report.data[index++] = _keyReport.modifiers;
    //report.data[index++] = _keyReport.reserved;

    for (int i = 0; i < REPORT_BYTES - 1; i++) {
        report.data[index++] = _keyReport.keys[i];
    }

    report.length = index;

    if (!send(&report)) {
        _mutex.unlock();
        return false;
    }

    _mutex.unlock();
    return true;
}


const uint8_t *KeyboardHID::report_desc() {

    static uint8_t reportDescriptor2[100];
    uint8_t index = 0;

    // USAGE_PAGE (Generic Desktop)
    reportDescriptor2[index++] = 0x05;
    reportDescriptor2[index++] = 0x01;

    // Usage (Keyboard)
    reportDescriptor2[index++] = 0x09;
    reportDescriptor2[index++] = 0x06;

    // Collection (Application)
    reportDescriptor2[index++] = 0xA1;
    reportDescriptor2[index++] = 0x01;

    // Report ID
    reportDescriptor2[index++] = 0x85;
    reportDescriptor2[index++] = DEFAULT_REPORT_ID_KEYBOARD;


    // Bitmap modifiers
    // Report Size (1)
    reportDescriptor2[index++] = 0x75;
    reportDescriptor2[index++] = 0x01;

    // Report Count (8)
    reportDescriptor2[index++] = 0x95;
    reportDescriptor2[index++] = 0x08;

    // Usage Page (Key Codes)
    reportDescriptor2[index++] = 0x05;
    reportDescriptor2[index++] = 0x07;

    // Usage Minimum (224)
    reportDescriptor2[index++] = 0x19;
    reportDescriptor2[index++] = 0xE0;

    // Usage Maximum (231)
    reportDescriptor2[index++] = 0x29;
    reportDescriptor2[index++] = 0xE7;

    // Logical Minimum (0)
    reportDescriptor2[index++] = 0x15;
    reportDescriptor2[index++] = 0x00;

    // Logical Maximum (1)
    reportDescriptor2[index++] = 0x25;
    reportDescriptor2[index++] = 0x01;

    // Input (Data, Variable, Absolute)
    reportDescriptor2[index++] = 0x81;
    reportDescriptor2[index++] = 0x02;


    // Bitmap of keys
    // Report Count ()
    reportDescriptor2[index++] = 0x95;
    reportDescriptor2[index++] = (REPORT_BYTES - 1) * 8;

    // Report Size (1)
    reportDescriptor2[index++] = 0x75;
    reportDescriptor2[index++] = 0x01;

    // Logical Minimum (0),
    reportDescriptor2[index++] = 0x15;
    reportDescriptor2[index++] = 0x00;

    // Logical Maximum(1)
    reportDescriptor2[index++] = 0x25;
    reportDescriptor2[index++] = 0x01;
    // Usage Page (Key Codes)
    reportDescriptor2[index++] = 0x05;
    reportDescriptor2[index++] = 0x07;

    // Usage Minimum (0)
    reportDescriptor2[index++] = 0x19;
    reportDescriptor2[index++] = 0x00;

    //  Usage Maximum ()
    reportDescriptor2[index++] = 0x29;
    reportDescriptor2[index++] = (REPORT_BYTES - 1) * 8 - 1;

    // Input (Data, Variable, Absolute)
    reportDescriptor2[index++] = 0x81;
    reportDescriptor2[index++] = 0x02;

    // END_COLLECTION
    reportDescriptor2[index++] = 0xc0;


    reportLength = index;
    return reportDescriptor2;
}


#define DEFAULT_CONFIGURATION (1)
#define TOTAL_DESCRIPTOR_LENGTH ((1 * CONFIGURATION_DESCRIPTOR_LENGTH) + (1 * INTERFACE_DESCRIPTOR_LENGTH) + (1 * HID_DESCRIPTOR_LENGTH) + (2 * ENDPOINT_DESCRIPTOR_LENGTH))

const uint8_t *KeyboardHID::configuration_desc(uint8_t index) {
    if (index != 0) {
        return nullptr;
    }
    uint8_t configuration_descriptor_temp[] = {
            CONFIGURATION_DESCRIPTOR_LENGTH, // bLength
            CONFIGURATION_DESCRIPTOR,        // bDescriptorType
            LSB(TOTAL_DESCRIPTOR_LENGTH),    // wTotalLength (LSB)
            MSB(TOTAL_DESCRIPTOR_LENGTH),    // wTotalLength (MSB)
            0x01,                            // bNumInterfaces
            DEFAULT_CONFIGURATION,           // bConfigurationValue
            0x00,                            // iConfiguration
            C_RESERVED | C_SELF_POWERED,     // bmAttributes
            C_POWER(0),                      // bMaxPower

            INTERFACE_DESCRIPTOR_LENGTH, // bLength
            INTERFACE_DESCRIPTOR,        // bDescriptorType
            0x00,                        // bInterfaceNumber
            0x00,                        // bAlternateSetting
            0x02,                        // bNumEndpoints
            HID_CLASS,                   // bInterfaceClass
            HID_SUBCLASS_BOOT,           // bInterfaceSubClass
            HID_PROTOCOL_KEYBOARD,       // bInterfaceProtocol
            0x00,                        // iInterface

            HID_DESCRIPTOR_LENGTH,                // bLength
            HID_DESCRIPTOR,                       // bDescriptorType
            LSB(HID_VERSION_1_11),                // bcdHID (LSB)
            MSB(HID_VERSION_1_11),                // bcdHID (MSB)
            0x00,                                 // bCountryCode
            0x01,                                 // bNumDescriptors
            REPORT_DESCRIPTOR,                    // bDescriptorType
            (uint8_t) (LSB(report_desc_length())), // wDescriptorLength (LSB)
            (uint8_t) (MSB(report_desc_length())), // wDescriptorLength (MSB)

            ENDPOINT_DESCRIPTOR_LENGTH, // bLength
            ENDPOINT_DESCRIPTOR,        // bDescriptorType
            _int_in,                    // bEndpointAddress
            E_INTERRUPT,                // bmAttributes
            LSB(MAX_HID_REPORT_SIZE),   // wMaxPacketSize (LSB)
            MSB(MAX_HID_REPORT_SIZE),   // wMaxPacketSize (MSB)
            1,                          // bInterval (milliseconds)

            ENDPOINT_DESCRIPTOR_LENGTH, // bLength
            ENDPOINT_DESCRIPTOR,        // bDescriptorType
            _int_out,                   // bEndpointAddress
            E_INTERRUPT,                // bmAttributes
            LSB(MAX_HID_REPORT_SIZE),   // wMaxPacketSize (LSB)
            MSB(MAX_HID_REPORT_SIZE),   // wMaxPacketSize (MSB)
            1,                          // bInterval (milliseconds)
    };
    MBED_ASSERT(sizeof(configuration_descriptor_temp) == sizeof(_configuration_descriptor));
    memcpy(_configuration_descriptor, configuration_descriptor_temp, sizeof(_configuration_descriptor));
    return _configuration_descriptor;
}

