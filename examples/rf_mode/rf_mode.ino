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

enum btn_action{
    BTN_ACTION_SHORT_PRESS = 0,
    BTN_ACTION_LONG_PRESS
};

void send_iot_msg();
enum btn_action btn_action_register();




void setup() {
    Serial.begin(115200);
    ebv_iot_init();
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    pinMode(PIN_BTN, INPUT);
    p("\n\rRF_MODE demo\n\r");

    uint8_t mode_counter = 0;
    uint8_t action_counter = 0;

    while(1){
        p("\n\rPress the button to perform actions\n\r");
        p("Press long the button to change rf_mode\n\r");
        
        enum btn_action btn_action = btn_action_register();

        switch(mode_counter){
            case 0:{
                if(btn_action == BTN_ACTION_LONG_PRESS){
                    p("\n\rChanging modem to NB-IoT\n\r");
                    // const uint8_t cfg_data[] = {0x91, 0x92, 0xA5, 0x6D, 0x6F, 0x64, 0x65, 0x6D, 0x81, 0xA7, 0x72, 0x66, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x00};
                    // esp_packet_t pkg;
                    // ebv_esp_packetBuilderByArray(&pkg, 0xAA, (uint8_t *)cfg_data, sizeof(cfg_data));
                    // if (ebv_esp_submitPacket(&pkg))
                    // {
                    //     esp_response_t resp;
                    //     ebv_esp_queryDelayedResponse(&resp);
                    // }
                    if( ebv_local_set_rf_mode(EBV_MODEM_RF_MODE_NBIOT) ){
                        mode_counter++;
                    }else {
                        p("Failed to changing modem to NB-IoT\n\r");
                    }
                } else {
                   send_iot_msg();
                }

                delay(1000);
                break;
            }
            case 1:{
                if(btn_action == BTN_ACTION_LONG_PRESS){
                    p("\n\rChanging modem to LTE-M\n\r");
                    // const uint8_t cfg_data[] = {0x91, 0x92, 0xA5, 0x6D, 0x6F, 0x64, 0x65, 0x6D, 0x81, 0xA7, 0x72, 0x66, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x01};
                    // esp_packet_t pkg;
                    // ebv_esp_packetBuilderByArray(&pkg, 0xAA, (uint8_t *)cfg_data, sizeof(cfg_data));
                    // if (ebv_esp_submitPacket(&pkg))
                    // {
                    //     esp_response_t resp;
                    //     ebv_esp_queryDelayedResponse(&resp);
                    // }
                    if( ebv_local_set_rf_mode(EBV_MODEM_RF_MODE_NBIOT) ){
                        mode_counter++;
                    }else {
                        p("Failed to changing modem to LTE-M\n\r");
                    }
                } else {
                    send_iot_msg();
                }
                delay(1000);
                break;
            }
            case 2:{
                if(btn_action == BTN_ACTION_LONG_PRESS){
                    p("\n\rChanging modem to GPS only\n\r");
                    // const uint8_t cfg_data[] = {0x91, 0x92, 0xA5, 0x6D, 0x6F, 0x64, 0x65, 0x6D, 0x81, 0xA7, 0x72, 0x66, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x02};
                    // esp_packet_t pkg;
                    // ebv_esp_packetBuilderByArray(&pkg, 0xAA, (uint8_t *)cfg_data, sizeof(cfg_data));
                    // if (ebv_esp_submitPacket(&pkg))
                    // {
                    //     esp_response_t resp;
                    //     ebv_esp_queryDelayedResponse(&resp);
                    // }
                    if( ebv_local_set_rf_mode(EBV_MODEM_RF_MODE_LTEM) ){
                        mode_counter++;
                    }else {
                        p("Failed to changing modem to GPS only\n\r");
                    }
                    action_counter = 0;
                } else {
                    if(action_counter == 0){
                        send_iot_msg();
                    } else if(action_counter == 1){
                        ebv_report_pvt();
                    }

                    action_counter++;
                    if(action_counter >= 2){
                        action_counter = 0;
                    }
                }

                delay(1000);
                break;
            }
            case 3:{
                if(btn_action == BTN_ACTION_LONG_PRESS){
                    p("\n\rChanging modem to OFFLINE mode\n\r");
                    // const uint8_t cfg_data[] = {0x91, 0x92, 0xA5, 0x6D, 0x6F, 0x64, 0x65, 0x6D, 0x81, 0xA7, 0x72, 0x66, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x03};
                    // esp_packet_t pkg;
                    // ebv_esp_packetBuilderByArray(&pkg, 0xAA, (uint8_t *)cfg_data, sizeof(cfg_data));
                    // if (ebv_esp_submitPacket(&pkg))
                    // {
                    //     esp_response_t resp;
                    //     ebv_esp_queryDelayedResponse(&resp);
                    // }
                    if( ebv_local_set_rf_mode(EBV_MODEM_RF_MODE_OFFLINE) ){
                        mode_counter++;
                    }else {
                        p("Failed to changing modem to OFFLINE mode\n\r");
                    }
                } else {
                    send_iot_msg();
                }

                delay(1000);
                break;
            }
            case 4:{
                if(btn_action == BTN_ACTION_LONG_PRESS){
                    p("\n\rChanging modem to RADIO TEST mode\n\r");
                    // const uint8_t cfg_data[] = {0x91, 0x92, 0xA5, 0x6D, 0x6F, 0x64, 0x65, 0x6D, 0x81, 0xA7, 0x72, 0x66, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x04};
                    // esp_packet_t pkg;
                    // ebv_esp_packetBuilderByArray(&pkg, 0xAA, (uint8_t *)cfg_data, sizeof(cfg_data));
                    // if (ebv_esp_submitPacket(&pkg))
                    // {
                    //     esp_response_t resp;
                    //     ebv_esp_queryDelayedResponse(&resp);
                    // }
                    if( ebv_local_set_rf_mode(EBV_MODEM_RF_MODE_RADIO_TEST) ){
                        mode_counter++;
                    }else {
                        p("Failed to changing modem to LTE-M\n\r");
                    }
                } else {
                    send_iot_msg();
                }

                delay(1000);
                break;
            }
    }

    if(mode_counter > 4){
        mode_counter = 0;
    }

    }
}

void loop(){}

enum btn_action btn_action_register(){
    // BTN is pressed
    while( digitalRead(PIN_BTN) );
    delay(100);
    volatile uint8_t cnt = 0;
    bool btn_pressed = true;
    while(cnt <= 30 && btn_pressed){
        if(digitalRead(PIN_BTN) ){
            btn_pressed = false;
        }

        cnt++;
        delay(100);
    }
    
    return cnt >= 30 ? BTN_ACTION_LONG_PRESS : BTN_ACTION_SHORT_PRESS;
}

void send_iot_msg(){
    p("Sending an event\n\r");
    ebv_iot_initGenericEvent("hello_cloud");
    ebv_iot_addGenericPayload("test", "rf_mode_change");
    bool ret = ebv_iot_submitGenericEvent();
    if(ret){
        p("Event sent\n\r");
    } else {
        esp_err_t err = ebv_iot_get_last_error_code();
        p("Sending event failed %d\n\r", err);

    }
}