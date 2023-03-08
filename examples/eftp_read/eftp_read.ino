#include <Arduino.h>
#include <HardwareSerial.h>
#include <extcwpack.h>
#include <SPI.h>
#include <SD.h>

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

#define FILE_DATA_TRANSMISSION_FRAME_LEN   IOT_MSG_MAX_LEN  // The maximum payload size of an esp packet 

#define REMOTE_FILE_NAME "47IUPj"                 // Name of the remote file
#define LOCAL_FILE_NAME "47IUPj"                  // Name of the file on the SD Card


EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void setup() {
    Serial.begin(115200);
    ebv_iot_init();
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    pinMode(PIN_BTN, INPUT);
    Serial.println("\n\reFTP sample");
    Serial.println("Press to start the file download...");
    while(digitalRead(PIN_BTN));
    Serial.println("Starting file download");
    Serial.print("Initializing SD card...");
    if (!SD.begin(10)) {
        Serial.println("initialization failed!");
        while (1);
    }
    Serial.println("SD ready.");
    File mFile;
    mFile = SD.open(LOCAL_FILE_NAME, FILE_WRITE);
    if(!mFile){
        Serial.println("Failed to open file to write on SD card");
        while(1);
    }
    p("Opening remote file %s to read", REMOTE_FILE_NAME);
    bool ret = ebv_eftp_open(REMOTE_FILE_NAME, "r");
    if(ret){
        char file_data[FILE_DATA_TRANSMISSION_FRAME_LEN] = {0};
        uint32_t received_data = 0;
        do {
            int ret = ebv_eftp_read(file_data, sizeof(file_data));
            if(ret < 0){
                // Error on file read
                ebv_esp_remote_file_error_codes err =  ebv_eftp_get_latest_error_code();
                p("Error while reading file: %d, closing down", err);
            } else {
                received_data += ret;
                p("Received %d bytes , in total: %d", ret, received_data);
                mFile.write(file_data, ret);
            }
        } while(ret == sizeof(file_data));
        
        ebv_eftp_close();
        Serial.println("File transfer DONE");
    } else {
        Serial.println("Failed to open remote file");
    }
}

void loop(){}