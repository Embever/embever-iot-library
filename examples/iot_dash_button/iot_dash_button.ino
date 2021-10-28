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

#define PIN_FETCH_BTN  2

#define LED_PIN                 3
#define DASHBTN_EVNT_TYPE         "buttonPressed"
#define DASHBTN_EVNT_KEY          "name"

#define ARDUINO_PIN_A2  16
#define ARDUINO_PIN_A3  17
#define PIN_EBV_IRQ     ARDUINO_PIN_A2
#define PIN_EBV_READY   ARDUINO_PIN_A3

void set_led(bool state){
    digitalWrite(LED_PIN, state);
}

EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void setup() {
    Serial.begin(9600);  // start serial for output
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\n\rDashButton demo starting...\n\r");
    pinMode(PIN_FETCH_BTN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    set_led(true);
    delay(1000);
    set_led(false);
    
}

void loop(){
    while( digitalRead(PIN_FETCH_BTN) );
    p("Sending event...\n\r");
    set_led(true);
    send_dash_event('1');
    waitForDevice();
    delay(5000);
    delay(5000);
    delay(5000);
    set_led(false);
}

void send_dash_event(char btn_id){
    ebv_iot_initGenericEvent(DASHBTN_EVNT_TYPE);
    ebv_iot_addGenericPayload(DASHBTN_EVNT_KEY, btn_id);
    ebv_iot_submitGenericEvent();
}