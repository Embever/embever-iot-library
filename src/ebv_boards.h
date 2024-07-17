#ifndef INC_EBV_BOARDS_H
#define INC_EBV_BOARDS_H

#if defined(__AVR__)
#define ARDUINO_AVR_PIN_A0 14
#define ARDUINO_AVR_PIN_A1 15
#define ARDUINO_AVR_PIN_A2 16
#define ARDUINO_AVR_PIN_A3 17
#endif

#define IOT_SHIELD_VERSION_1    0   // IoT shield with a button and an LED
#define IOT_SHIELD_VERSION_2    1   // IoT shield with a button, LED, ACC / LIGHT / TEMP Sensor and Cooling FAN control


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
    #define DEFAULT_PIN_EBV_READY         A3
    #define DEFAULT_PIN_EBV_IRQ           A2
    #define DEFAULT_PIN_BTN                5
    #define DEFAULT_PIN_LED                4
    #define DEFAULT_PIN_ONBOARD_BTN_B1  PC13        // Active Low
    #define DEFAULT_PIN_ONBOARD_LED_D1  PB13
#endif

#ifndef IOT_SHIELD_VERSION
#define IOT_SHIELD_VERSION IOT_SHIELD_VERSION_1
#endif

#if     IOT_SHIELD_VERSION == IOT_SHIELD_VERSION_1
#pragma message("Compiling to use IoT Shield Version 1")
#define IOT_SHIELD_PIN_EBV_READY    A3
#define IOT_SHIELD_PIN_EBV_IRQ      A2
#define IOT_SHIELD_PIN_BTN           5
#define IOT_SHIELD_PIN_LED           4
#elif   IOT_SHIELD_VERSION == IOT_SHIELD_VERSION_2
#pragma message("Compiling to use IoT Shield Version 2")
#define IOT_SHIELD_PIN_EBV_READY    10                    // ESP IRQ signal connected here
#define IOT_SHIELD_PIN_EBV_IRQ      11                    // ESP READY signal connected here
#define IOT_SHIELD_PIN_BTN           2                    // Push button, with pullup resistor, the btn pulling the signal low
#define IOT_SHIELD_PIN_LED          A1                    // LED, active LOW
#endif

#ifndef PIN_EBV_READY
#define PIN_EBV_READY   IOT_SHIELD_PIN_EBV_READY
#endif

#ifndef PIN_EBV_IRQ
#define PIN_EBV_IRQ     IOT_SHIELD_PIN_EBV_IRQ
#endif

#ifndef PIN_BTN
#define PIN_BTN         IOT_SHIELD_PIN_BTN
#endif

#ifndef PIN_LED
#define PIN_LED         IOT_SHIELD_PIN_LED
#endif


#endif