// Copyright (c) 2023. Leonhard Baschang

#ifndef HIDKEYBOARD_HIDKEYBOARD_H
#define HIDKEYBOARD_HIDKEYBOARD_H


#define DEFAULT_REPORT_ID_KEYBOARD 0x02
#define DEFAULT_DEVICE_ID_KEYBOARD 0x06

#define REPORT_BYTES 16


#include "PluggableUSBHID.h"
#include "platform/Stream.h"
#include "PlatformMutex.h"



//================================================================================
//================================================================================
//  Keyboard
#include "keyboard_layout.h"




typedef struct {
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keys[REPORT_BYTES];
} KeyReport;


namespace arduino {
    class KeyboardHID : public USBHID {
        KeyReport _keyReport;
        const uint16_t *_asciimap;
        uint16_t asciimapSize;


        bool autoSend = true;

        uint16_t getKeycode(uint16_t k);

    public:
        KeyboardHID();

        bool sendState();

        void press(uint16_t k, bool isKeyCode = false);

        void release(uint16_t k, bool isKeyCode = false);

        void releaseAll();

        void pressAndRelease(uint16_t k, bool isKeyCode = false);


    protected:
        const uint8_t *configuration_desc(uint8_t index) override;

    private:
        uint8_t _configuration_descriptor[41];
        PlatformMutex _mutex;

        const uint8_t *report_desc() override;


    };
};


#endif //HIDKEYBOARD_HIDKEYBOARD_H
