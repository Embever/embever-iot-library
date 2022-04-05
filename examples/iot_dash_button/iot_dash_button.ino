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
#include "Wire/EBV_Wire.h"
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
    p("\n\rDashButton demo starting...\n\r");
    pinMode(PIN_BTN, INPUT);
    pinMode(PIN_LED, OUTPUT);
    set_led(true);
    delay(1000);
    set_led(false);
    
}

void loop(){
    while( digitalRead(PIN_BTN) );
    p("Sending event...\n\r");
    set_led(true);
    bool ret = send_dash_event('1');
    if(ret){
        p("Event sent\n\r");
    } else {
        p("Sending event failed\n\r");
    }
    waitForDevice();
    delay(1000);
    set_led(false);
}

bool send_dash_event(char btn_id){
    ebv_iot_initGenericEvent(DASHBTN_EVNT_TYPE);
    ebv_iot_addGenericPayload(DASHBTN_EVNT_KEY, btn_id);
    return ebv_iot_submitGenericEvent();
}