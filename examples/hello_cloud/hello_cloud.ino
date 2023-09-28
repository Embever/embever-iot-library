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

#define USR_BTN_AVAILABLE 1             // Set to 1 if there is button available on your hardware

EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void print_fail_reason(){
#if EBV_STRINGIFY_EN == 1       // defined on ebv_log_conf.h header file
    EBV_ESP_GET_LAST_ERROR( esp_err_str );
    ebv_local_device_status_t device_status;
    ebv_local_status_update(&device_status);
    char general_status[48];
    ebv_local_status_general_str(&(device_status.general_status),general_status );
    p("ESP error : %s, device status : %s \n\r", esp_err_str, general_status);
    
#endif
}

void setup() {
    Serial.begin(115200);
    ebv_iot_init();
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\n\rHello Cloud starting\n\r");
    p("Preparing hello_cloud event\n\r");
    ebv_iot_initGenericEvent("Hi_Cloud");               // Set the event type
    ebv_iot_addGenericPayload("source", "AppMCU");      // Set payload
#if USR_BTN_AVAILABLE == 1
    pinMode(PIN_BTN, INPUT);
    while(1){
        p("Press the user button to send an event\n\r");
        while( digitalRead(PIN_BTN) );
#endif
        if( ebv_util_wait_device_ready(DEFAULT_NETWORK_ATTACH_TIMEOUT_SEC) == false){
            p("Device is not ready, timeout reached\n\r");
            print_fail_reason();
            while(1);
        }
        p("Sending hello_cloud event...\n\r");
        bool ret = ebv_iot_submitGenericEvent();            // Send event
        if(ret){
            p("Event sent\n\r");
        } else {
           p("Sending event failed\n\r");
           print_fail_reason();
        }
#if USR_BTN_AVAILABLE == 1
    delay(2000);
    }
#endif
}

void loop(){}