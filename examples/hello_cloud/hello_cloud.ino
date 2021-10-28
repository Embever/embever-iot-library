// To make this sketch work, you need to change the following C macros
//
// Wire.h #define BUFFER_LENGTH 32      --> #define BUFFER_LENGTH 128
// twi.h  #define TWI_BUFFER_LENGTH 32  --> #define TWI_BUFFER_LENGTH 128
// The predefined buffers for sending data are just too small, and there is no way to change them

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <extcwpack.h>

#include "ebv_iot.h"
#include "print_serial.h"

#define ARDUINO_PIN_A2  16
#define ARDUINO_PIN_A3  17
#define PIN_EBV_IRQ     ARDUINO_PIN_A2
#define PIN_EBV_READY   ARDUINO_PIN_A3


EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void setup() {
    Serial.begin(9600);
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\n\rHello Cloud starting...\n\r");
    p("\n\rSending hello_cloud event...\n\r");
    ebv_iot_initGenericEvent("Hello_Cloud");                // Set the event type
    ebv_iot_addGenericPayload("source", "ebv_demo");        // Set payload
    bool ret = ebv_iot_submitGenericEvent();                // Send event
    if(ret){
        p("Event sent to the cloud\n\r");
    } else {
        p("No response from device\n\r");
    }
}

void loop(){}
