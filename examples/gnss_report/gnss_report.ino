#include <Arduino.h>
#include <HardwareSerial.h>

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
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\n\rGNSS Report sample starting\n\r");
}

void loop(){
    while( digitalRead(PIN_BTN) );
    bool ret = ebv_local_report_gnss();
    if(ret){
        p("Report request submitted\n\r");
    } else {
        p("Failed to submit report request\n\r");
    }
    bool has_result = false;
    ebv_gnss_status_report_t status;
    while(!has_result){
        delay(30 * 1000);
        ebv_local_query_gnss_status(&status);
        if( status.gps_status == EBV_GNSS_STATUS_STOPPED){
            has_result = true;
        }
    }
    if(status.is_last_fix_success){
        p("Location data sent to the cloud\n\r");
    } else {
        p("GPS fix timeout, location data not sent\n\r");
    }
}