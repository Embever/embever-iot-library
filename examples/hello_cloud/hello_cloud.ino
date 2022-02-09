// To make this sketch work, you need to change the following C macros
//
// Wire.h #define BUFFER_LENGTH 32      --> #define BUFFER_LENGTH 128
// twi.h  #define TWI_BUFFER_LENGTH 32  --> #define TWI_BUFFER_LENGTH 128
// The predefined buffers for sending data are just too small, and there is no way to change them

#include <Arduino.h>
#include <HardwareSerial.h>
#include <extcwpack.h>

// Usefull definitions
#define ARDUINO_AVR_PIN_A0 14
#define ARDUINO_AVR_PIN_A1 15
#define ARDUINO_AVR_PIN_A2 16
#define ARDUINO_AVR_PIN_A3 17

// Uncomment this macros to assign custom GPIO pins
// #define PIN_EBV_IRQ     ARDUINO_AVR_PIN_A0
// #define PIN_EBV_READY   ARDUINO_AVR_PIN_A1

#include "ebv_iot.h"
#include "print_serial.h"
#include "Wire/EBV_Wire.h"

EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void setup() {
    Serial.begin(9600);
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\n\rHello Cloud starting...\n\r");
    p("\n\rPreparing hello_cloud event...\n\r");
    ebv_iot_initGenericEvent("Hi_Cloud");                           // Set the event type
    ebv_iot_addGenericPayload("source", "ESP");                     // Set payload
    p("\n\rSending hello_cloud event...\n\r");
    bool ret = ebv_iot_submitGenericEvent();                        // Send event
    if(ret){
        p("Event sent to the cloud\n\r");
    } else {
        p("No response from device\n\r");
    }
}

void loop(){}