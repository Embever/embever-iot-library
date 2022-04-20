#include <Arduino.h>
#include <HardwareSerial.h>
#include <extcwpack.h>

// Define the following macros to assign custom GPIO pins for each functions
// The default configuration is the following:
// Arduino pin A3 -- PIN_ESP READY
// Arduino pin A2 -- PIN_ESP_IRQ
// Arduino pin  5 -- PIN_BTN
// Arduino pin  4 -- PIN_BTN

// #define PIN_EBV_IRQ      ARDUINO_AVR_PIN_A2       // ESP IRQ signal connected here
// #define PIN_EBV_READY    ARDUINO_AVR_PIN_A3       // ESP READY signal connected here
// #define PIN_BTN                           5       // Push button, with pullup resistor, the btn pulling the signal low
// #define PIN_LED                           4       // LED, active HIGH

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