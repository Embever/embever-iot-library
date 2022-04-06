#include <Arduino.h>
#include <HardwareSerial.h>
#include <extcwpack.h>

// Define the following macros to assign custom GPIO pins for each functions
// The default configuration is the following:
// Arduino pin A3 -- PIN_ESP READY
// Arduino pin A2 -- PIN_ESP_IRQ
// Arduino pin  2 -- PIN_BTN
// Arduino pin 13 -- PIN_LED

// #define PIN_EBV_IRQ      ARDUINO_AVR_PIN_A2       // ESP IRQ signal connected here
// #define PIN_EBV_READY    ARDUINO_AVR_PIN_A3       // ESP READY signal connected here
// #define PIN_BTN                           2       // Push button, with pullup resistor, the btn pulling the signal low
// #define PIN_LED                          13       // LED, active HIGH

#include "ebv_iot.h"
#include "print_serial.h"
#include "Wire.h"
#include "ebv_boards.h"

#define LED_ACTION_TYPE         "setLED"
#define LED_ACTION_KEY          "state"
#define LED_ACTION_VALUE_TRUE   "on"
#define LED_ACTION_VALUE_FALSE  "off"

static bool LED_state;

bool parseLEDstate(ebv_action_t *a, bool *led_state);

void set_led(bool state){
    digitalWrite(PIN_LED, state);
}

EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void setup() {
    Serial.begin(115200);  // start serial for output
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\n\rIoT Blinky starting\n\r");
    pinMode(PIN_BTN, INPUT);
    pinMode(PIN_LED, OUTPUT);
    set_led(true);
    delay(1000);
    set_led(false);
    LED_state = false;
    while( digitalRead(PIN_BTN) );
}

void loop(){
    p("Fetching\n\r");
    esp_response_t response;
    bool ret;
    ebv_ret_t ebv_ret;
    ebv_ret = ebv_iot_receiveAction(&response);
    if( ebv_ret == EBV_RET_NO_ACTION ){
        p("No action for this device\n\r");
        goto end;
    } else if( ebv_ret != EBV_RET_OK ){
        p("Failed to read actions\n\r");
        goto end;
    }
    ebv_action_t action;
    p("Actions received\n\r");
    ret = ebv_iot_parseAction(&response, &action);
    if(!ret){
        p("Error during parsing action\n\r");
        goto end;
    }
    if( !strncmp(action.type, LED_ACTION_TYPE, strlen(LED_ACTION_TYPE)) ){
        ret = parseLEDstate(&action, &LED_state);
        if(!ret){
            p("Error during parsing led action payload\n\r");
            goto end;
        }
        p("Payload parsed\n\r");
        set_led( LED_state );
        p("Submitting action result\n\r");
        submitLEDstate(&action, LED_state);
    } else {
        if( action.type[0] != 0){
            p("Another action received : %s\n\r", action.type);
        }
    }
end:
    memset(&action, 0, sizeof(ebv_action_t));
    waitForDevice();
    delay(5000);
    // while( digitalRead(PIN_BTN) );
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
    if(isLedOn){
        ebv_iot_addGenericPayload("led", "on");
    } else {
        ebv_iot_addGenericPayload("led", "off");
    }
    esp_response_t resp;
    ebv_ret_t res = ebv_iot_submitGenericActionResult(a, &resp);
    if(res == EBV_RET_OK){
        return true;
    }
    return false;
}