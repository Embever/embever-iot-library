#include <Arduino.h>
#include <HardwareSerial.h>
#include <extcwpack.h>

// Define the following macros to assign custom GPIO pins for READY and IRQ lines
// The default configuration is the following:
// Arduino pin A3 -- ESP READY
// Arduino pin A2 -- ESP IRQ
// #define PIN_EBV_IRQ     ARDUINO_AVR_PIN_A0       // ESP IRQ signal connected here
// #define PIN_EBV_READY   ARDUINO_AVR_PIN_A1       // ESP IRQ signal connected here

#include "ebv_iot.h"
#include "print_serial.h"
#include "Wire.h"
#include "ebv_boards.h"

EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void setup() {
    Serial.begin(115200);
    ebv_iot_init();
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\n\rHello Cloud starting\n\r");
    p("Preparing hello_cloud event\n\r");
    ebv_iot_initGenericEvent("Hi_Cloud");               // Set the event type
    ebv_iot_addGenericPayload("source", "AppMCU");      // Set payload
    p("Sending hello_cloud event...\n\r");
    bool ret = ebv_iot_submitGenericEvent();            // Send event
    if(ret){
        p("Event sent\n\r");
    } else {
       p("Sending event failed\n\r");
    }
}

void loop(){}