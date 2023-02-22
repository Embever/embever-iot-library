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

#define DASHBTN_EVNT_TYPE   "buttonPressed"
#define DASHBTN_EVNT_KEY    "name"

bool send_dash_event(char btn_id);

void set_led(bool state){
    digitalWrite(PIN_LED, state);
}

EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void setup() {
    Serial.begin(115200);  // start serial for output
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\n\rDashButton demo starting\n\r");
    pinMode(PIN_BTN, INPUT);
    pinMode(PIN_LED, OUTPUT);
    set_led(true);
    delay(1000);
    set_led(false);
}

void loop(){
    while( digitalRead(PIN_BTN) );
    p("Sending event\n\r");
    set_led(true);
    bool ret = send_dash_event('1');
    if(ret){
        p("Event sent\n\r");
    } else {
        esp_err_t err = ebv_iot_get_last_error_code();
        char esp_err_str[32];
        ebv_iot_esp_err_str(err, esp_err_str);
        p("Sending event failed (%d) %s\n\r", err, esp_err_str);
    }
    delay(1000);
    set_led(false);
}

bool send_dash_event(char btn_id){
    ebv_iot_initGenericEvent(DASHBTN_EVNT_TYPE);
    ebv_iot_addGenericPayload(DASHBTN_EVNT_KEY, btn_id);
    return ebv_iot_submitGenericEvent();
}