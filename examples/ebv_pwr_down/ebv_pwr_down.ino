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

bool send_sample_event();
void delay_sec(uint8_t seconds);
void delay_min(uint8_t minutes);
void print_fail_reason();

void setup() {
    Serial.begin(115200);
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\r\nPower down mode test sample\r\n");
    while( digitalRead(PIN_BTN) );
}

void loop(){
    p("Sending sample event\r\n");

    if( ebv_util_wait_device_ready(DEFAULT_NETWORK_ATTACH_TIMEOUT_SEC) == false){
        p("Device is not ready, timeout reached\n\r");
        print_fail_reason();
        while(1);
    }

    bool ret = send_sample_event();
    if(ret){
        p("Event sent\r\n");
    } else {
        p("Failed to send event, retrying...\r\n");
        delay_sec(30);
        return;
    }
    p("Put CaaM board to power down mode and wait 1 minute...\r\n");
    ret = ebv_local_set_op_mode(EBV_OP_MODE_PWR_DOWN);
    if(!ret){
        p("Failed to activate sleep mode\r\n");
        delay_sec(10);
        return;
    }
    p("Sleep mode activated\r\n");
    delay_min(1);
    p("Put CaaM board back to online mode...\r\n");
    ebv_local_set_op_mode(EBV_OP_MODE_ONLINE);
}

bool send_sample_event(){
    ebv_iot_initGenericEvent("sample");
    ebv_iot_addGenericPayload("src", "app_mcu");
    return ebv_iot_submitGenericEvent();
}

void delay_sec(uint8_t seconds){
    for(int i = 0; i < seconds; i++){
        delay(1000);
    }
}

void delay_min(uint8_t minutes){
    for(int i = 0; i < minutes; i++){
        delay_sec(60);
    }
}

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