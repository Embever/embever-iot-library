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
#define LED_ACTION_TYPE         "setLED"
#define LED_ACTION_KEY          "state"
#define LED_ACTION_VALUE_TRUE   "on"
#define LED_ACTION_VALUE_FALSE  "off"

#define ARDUINO_PIN_A2  16
#define ARDUINO_PIN_A3  17
#define PIN_EBV_IRQ     ARDUINO_PIN_A2
#define PIN_EBV_READY   ARDUINO_PIN_A3

static bool LED_state;

bool parseLEDstate(ebv_action_t *a, bool *led_state);

void set_led(bool state){
    digitalWrite(LED_PIN, state);
}

EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void setup() {
    Serial.begin(9600);  // start serial for output
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\n\rIoT Blinky starting...\n\r");
    pinMode(PIN_FETCH_BTN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    set_led(true);
    delay(1000);
    set_led(false);
    LED_state = false;
    while( digitalRead(PIN_FETCH_BTN) );
}

void loop(){
    p("Fetching...\n\r");
    esp_response_t response;
    ebv_iot_receiveAction(&response);
    uint8_t i;
    // for(i = 0; i < response.response_len; i++){
    //     if( !(i % 8 )){ Serial.print("\n\r"); }
    //     // p("%x ", response.response[i]);
    // }
    if(response.response_len == 0){
        // p("No action for this device\n\r");
        return;
    }
    ebv_action_t action;
    // // p("\n\rActions received\n\r");
    bool ret = ebv_iot_parseAction(&response, &action);
    if(!ret){
        // p("Error during parsing action\n\r");
        return;
    }
    if( !strncmp(action.type, LED_ACTION_TYPE, strlen(LED_ACTION_TYPE)) ){
        ret = parseLEDstate(&action, &LED_state);
        if(!ret){
            // p("Error during parsing led action payload\n\r");
            return;
        }
        // p("Payload parsed\n\r");
        // LED_state = !LED_state;
        set_led( LED_state );
        submitLEDstate(&action, LED_state);
    } else {
        if( action.type[0] != 0){
            // p("Another action received : %s\n\r", action.type);
        }
    }

    memset(&action, 0, sizeof(ebv_action_t));
    waitForDevice();
    delay(5000);
    delay(5000);
    delay(5000);
}

esp_response_t resp;



bool parseLEDstate(ebv_action_t *a, bool *led_state){
    cw_unpack_context uc;
    cw_unpack_context_init(&uc, a->payload, a->payload_len, NULL);
    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_MAP){ return false; }
    cw_unpack_next(&uc);        // key = state
    if(uc.item.type != CWP_ITEM_STR){ return false; }
    cw_unpack_next(&uc);        // value
    if(uc.item.type != CWP_ITEM_STR){ return false; }
    if(!strncmp(LED_ACTION_VALUE_TRUE, (char *) uc.item.as.str.start, uc.item.as.str.length)){
        *led_state = true;
        return true;
    }
    if(!strncmp(LED_ACTION_VALUE_FALSE, (char *) uc.item.as.str.start, uc.item.as.str.length)){
        *led_state = false;
        return true;
    }
    return false;
}

bool submitLEDstate(ebv_action_t *a, bool isLedOn){
    a->result = true;
    ebv_iot_initGenericResponse();
    p("init GR\n\r");
    if(isLedOn){
        ebv_iot_addGenericPayload("LED", "on");
    } else {
        ebv_iot_addGenericPayload("LED", "off");
    }
    esp_response_t resp;
    bool res = ebv_iot_submitGenericActionResult(a, &resp);
    if(!res){
        // p("FAIL\n\r");
    }
    return res;
}