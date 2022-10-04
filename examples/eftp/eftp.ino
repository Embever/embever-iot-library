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

#define FILE_DATA_TRANSMISSION_FRAME_LEN   128

EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void setup() {
    Serial.begin(115200);
    ebv_iot_init();
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\n\reFTP sample\n\r");
    bool ret = ebv_eftp_open("app_mcu_file", "w");
    if(ret){
        //char file_data[] = "This file came from the APP mcu over esp";
        char file_data[1 * 1024];
        unsigned int index;
        const int file_len = sizeof(file_data);
        for(index = 0; index < file_len; index++){
            file_data[index] = index % 256;
        }
        index = 0;
        do {
            const int write_len = (index + FILE_DATA_TRANSMISSION_FRAME_LEN) <= file_len ? FILE_DATA_TRANSMISSION_FRAME_LEN : file_len % FILE_DATA_TRANSMISSION_FRAME_LEN; 
            bool ret = ebv_eftp_write(&file_data[index], write_len);
            if(ret){
                // Success
                index += write_len;
                p("File write: %d / %d\n\r", index, file_len);
            } else {
                ebv_esp_remote_file_error_codes err =  ebv_eftp_get_latest_error_code();
                if(err == EBV_ESP_REMOTE_FILE_ERROR_RESOURCE_BUSY){
                    // Need to wait a bit, the file buffer is full on the CaaM board
                    delay(100);
                } else {
                    // Something else
                    p("Unknown error during file transfer: %d\n\r", err);
                    break;
                }
            }
        } while(index < sizeof(file_data));
        
        ebv_eftp_close();
        p("File transfer DONE\n\r");
    } else {
        p("Failed to open remote file\n\r");
    }
}

void loop(){}