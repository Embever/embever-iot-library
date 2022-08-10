#include <Arduino.h>
#include <HardwareSerial.h>
#include <extcwpack.h>

//  ######### Complex IoT Payload Demo #########
//  This sample application going to send the following data to the cloud (in json format):
// {
//         "url": "https://api.embever.com/v2/events/....",
//         "id": XYZ,
//         "device": "XYZ",
//         "sim": "XYZ",
//         "type": "Hi_Cloud",
//         "payload": {
//             "k": "v",
//             "key": "value",
//             "values": [
//                 1,
//                 2,
//                 3,
//                 4,
//                 5
//             ]
//         },
//         "created_at": "XYZ"
//     },

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
    ebv_iot_initGenericEvent("Hi_Cloud");                   // Set the event type

    const uint8_t a = 1, b = 2, c = 3, d = 4, e = 5;        // constant variables for demonstrating
    EBV_IOT_BUILD_UINT_LIST(uint_list, a, b, c, d, e);      // build the integer list list into the uint_list variable
    if(uint_list.buf_len == 0){                             // Check the result, the buf_len should greater that 0
        p("Error during creating uint_list");
        return;
    }

    ebv_iot_addGenericPayload("key", "value");              // Add payload
    ebv_iot_addGenericPayload("values", &uint_list);        // Add the integer list payload
    ebv_iot_addGenericPayload("k", "v");                    // Add more data
    p("Sending hello_cloud event...\n\r");
    bool ret = ebv_iot_submitGenericEvent();                // Send event
    if(ret){
        p("Event sent\n\r");
    } else {
       p("Sending event failed\n\r");
    }
}

void loop(){}