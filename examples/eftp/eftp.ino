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

#define FILE_DATA_TRANSMISSION_FRAME_LEN   (IOT_MSG_MAX_LEN - ESP_PACKET_OVERHEAD)

#define USE_STATIC_DATA         0
#define GENERATE_FILE_CONTENT   0
#define USE_SD_CARD             1
#define SD_FILE_NAME            "ebv_esp.pdf"

#if USE_SD_CARD + GENERATE_FILE_CONTENT + USE_STATIC_DATA > 1
#error "Only one feature can be enabled"
#endif

EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void setup() {
    Serial.begin(115200);
    ebv_iot_init();
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    pinMode(PIN_BTN, INPUT);
    Serial.println("\n\reFTP sample");
    Serial.println("Press to start the file upload...");
    while(digitalRead(PIN_BTN));
    Serial.println("Starting file upload");
#if USE_SD_CARD == 1
    bool ret = ebv_eftp_open(SD_FILE_NAME, "w");
#else
    bool ret = ebv_eftp_open("app_mcu_file", "w");
#endif
    if(ret){
#if USE_STATIC_DATA == 1
        char file_data[] = "This file came from the APP mcu over esp";
        const unsigned int file_len = sizeof(file_data);
#endif
#if GENERATE_FILE_CONTENT == 1
        char file_data[4 * 1024];
        const int file_len = sizeof(file_data);
        for(index = 0; index < file_len; index++){
            file_data[index] = index % 256;
        }
        const unsigned int file_len = sizeof(file_data);
#endif
#if USE_SD_CARD == 1
    Serial.print("Initializing SD card...");
    if (!SD.begin(10)) {
        Serial.println("initialization failed!");
        while (1);
    }
    Serial.println("initialization done.");
    File mFile;
    mFile = SD.open(SD_FILE_NAME, FILE_READ);
    if(!mFile){
        Serial.println("Failed to open file from SD card");
        while(1);
    }
    char file_data[FILE_DATA_TRANSMISSION_FRAME_LEN] = {0};
    const uint32_t file_len = mFile.size();
#endif
        uint32_t index = 0;
        do {
#if USE_SD_CARD == 1
            const unsigned int write_len = mFile.read(file_data, sizeof(file_data));
#elif USE_STATIC_DATA == 1 || GENERATE_FILE_CONTENT == 1
            const int write_len = (index + FILE_DATA_TRANSMISSION_FRAME_LEN) <= file_len ? FILE_DATA_TRANSMISSION_FRAME_LEN : file_len % FILE_DATA_TRANSMISSION_FRAME_LEN; 
#endif
            // send the data to the CaaM
            volatile bool ret;
            do{
#if USE_SD_CARD == 1
                ret = ebv_eftp_write(file_data, write_len);
#elif USE_STATIC_DATA == 1 || GENERATE_FILE_CONTENT == 1
                ret = ebv_eftp_write(&file_data[index], write_len);
#endif
                if(ret){
                    // Success
                    index += write_len;
                    p("File write: %d / %d\n\r", index, file_len);
                } else {
                    ebv_esp_remote_file_error_codes err =  ebv_eftp_get_latest_error_code();
                    if(err == EBV_ESP_REMOTE_FILE_ERROR_RESOURCE_BUSY){
                        // Need to wait a bit, the file buffer is full on the CaaM side
                        Serial.println("Resource busy, retrying...");
                        delay(1000);
                    } else {
                        // Something else
                        p("Unknown error during file transfer: %d\n\r", err);
                        break;
                    }
                }
            } while(!ret);
        } while(index < file_len);
        
        ebv_eftp_close();
        Serial.println("File transfer DONE");
    } else {
        Serial.println("Failed to open remote file");
    }
}

void loop(){}