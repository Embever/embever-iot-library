#ifndef INC_EBV_BOARDS_H
#define INC_EBV_BOARDS_H

#ifndef PIN_BTN
    #if defined(ESP32)
        #define PIN_BTN 14
    #elif defined(__AVR__)
        #define PIN_BTN  2
    #else
        #error "Missing macro definition: PIN_FETCH_BTN"
    #endif
#endif

#ifndef PIN_LED
    #if defined(ESP32)
        #define PIN_LED 12
    #elif defined(__AVR__)
        #define PIN_LED  13
    #else
        #error "Missing macro definition: LED_PIN"
    #endif
#endif

#if defined(__AVR__)
// Usefull definitions
#define ARDUINO_AVR_PIN_A0 14
#define ARDUINO_AVR_PIN_A1 15
#define ARDUINO_AVR_PIN_A2 16
#define ARDUINO_AVR_PIN_A3 17
#endif

#endif