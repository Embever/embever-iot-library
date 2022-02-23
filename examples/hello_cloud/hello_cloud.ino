#include <Arduino.h>
#include <HardwareSerial.h>
#include <extcwpack.h>

// Usefull definitions
#define ARDUINO_AVR_PIN_A0 14
#define ARDUINO_AVR_PIN_A1 15
#define ARDUINO_AVR_PIN_A2 16
#define ARDUINO_AVR_PIN_A3 17

// Define the following macros to assign custom GPIO pins for READY and IRQ lines
// The default configuration is the following:
// Arduino pin A3 -- ESP READY
// Arduino pin A2 -- ESP IRQ
// #define PIN_EBV_IRQ     ARDUINO_AVR_PIN_A0       // ESP IRQ signal connected here
// #define PIN_EBV_READY   ARDUINO_AVR_PIN_A1       // ESP IRQ signal connected here

#include "ebv_iot.h"
#include "print_serial.h"
#include "Wire/EBV_Wire.h"

EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void setup() {
    Serial.begin(115200);
    ebv_iot_init();
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\n\rHello Cloud starting...\n\r");
    p("\n\rPreparing hello_cloud event...\n\r");
    ebv_iot_initGenericEvent("Hi_Cloud");                          // Set the event type
    ebv_iot_addGenericPayload("source", "AppMCU");             // Set payload
    p("\n\rSending hello_cloud event...\n\r");
    bool ret = ebv_iot_submitGenericEvent();                       // Send event
    if(ret){
        p("Event sent to the cloud\n\r");
    } else {
        p("No response from device\n\r");
    }
}

void loop(){}