#ifndef INC_EBV_BOARDS_H
#define INC_EBV_BOARDS_H

#if defined(__AVR__)
#define ARDUINO_AVR_PIN_A0 14
#define ARDUINO_AVR_PIN_A1 15
#define ARDUINO_AVR_PIN_A2 16
#define ARDUINO_AVR_PIN_A3 17
#endif


#if defined(ESP32)
    #define DEFAULT_PIN_EBV_READY   23
    #define DEFAULT_PIN_EBV_IRQ     19
    #define DEFAULT_PIN_BTN         14
    #define DEFAULT_PIN_LED         12
#elif defined (__AVR__)
    #define DEFAULT_PIN_EBV_READY   ARDUINO_AVR_PIN_A3
    #define DEFAULT_PIN_EBV_IRQ     ARDUINO_AVR_PIN_A2
    #define DEFAULT_PIN_BTN         5
    #define DEFAULT_PIN_LED         4
#elif defined(STM32F103xB)
    #define DEFAULT_PIN_EBV_READY   A3
    #define DEFAULT_PIN_EBV_IRQ     A2
    #define DEFAULT_PIN_BTN         5
    #define DEFAULT_PIN_LED         4
#endif


#ifndef PIN_EBV_READY
#define PIN_EBV_READY DEFAULT_PIN_EBV_READY
#endif

#ifndef PIN_EBV_IRQ
#define PIN_EBV_IRQ DEFAULT_PIN_EBV_IRQ
#endif

#ifndef PIN_BTN
#define PIN_BTN DEFAULT_PIN_BTN
#endif

#ifndef PIN_LED
#define PIN_LED DEFAULT_PIN_LED
#endif

#endif