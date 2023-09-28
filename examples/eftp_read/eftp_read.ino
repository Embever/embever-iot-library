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

#define FILE_DATA_TRANSMISSION_FRAME_LEN   230

// #define REMOTE_FILE_NAME "47IUPj"                 // Name of the remote file
// #define LOCAL_FILE_NAME "47IUPj"                  // Name of the file on the SD Card

// #define REMOTE_FILE_NAME "jBW0iERaO"                 // Name of the remote file, 70kb in size
// #define LOCAL_FILE_NAME "CaaM.pdf"                   // Name of the file on the SD Card
#define REMOTE_FILE_NAME "Au3mHIQqO"                    // Name of the remote file, 381024kb in size
#define LOCAL_FILE_NAME "ESP.pdf"                       // Name of the file on the SD Card



EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void print_fail_reason();

void setup() {
    Serial.begin(115200);
    ebv_iot_init();
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    pinMode(PIN_BTN, INPUT);
    Serial.println("\n\reFTP sample");
    Serial.println("Press to start the file download...");
    while(digitalRead(PIN_BTN));
    if( ebv_util_wait_device_ready(DEFAULT_NETWORK_ATTACH_TIMEOUT_SEC) == false){
        p("Device is not ready, timeout reached\n\r");
        print_fail_reason();
        while(1);
    }
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
    p("Opening remote file %s to read\n\r", REMOTE_FILE_NAME);
    bool ret = ebv_eftp_open(REMOTE_FILE_NAME, "r");
    if(ret){
        char file_data[FILE_DATA_TRANSMISSION_FRAME_LEN] = {0};
        uint32_t total_received_data = 0;
        int received_data = 0;
        unsigned int total_file_len = ebv_eftp_get_current_file_len();
        p("File length : %d\n\r", total_file_len);
        do {
            received_data = ebv_eftp_read(file_data, sizeof(file_data));
            if(received_data < 0){
                // Error on file read
                esp_err_t err =  ebv_iot_get_last_error_code();
                p("Error while reading file: %d, closing down\n\r", err);
                char esp_err_buffer[48] = {0};
                ebv_iot_esp_err_str(err, esp_err_buffer);
                p("ESP error code: %s\n\r",esp_err_buffer);
            } else {
                total_received_data += received_data;
                p("Received %d bytes , in total: %d\n\r", received_data, total_received_data);
                mFile.write(file_data, received_data);
            }
        } while(received_data >= 0);
        ebv_eftp_close();
        mFile.close();
        Serial.println("File transfer finished");

        if(total_received_data == total_file_len){
            p("File download successful\n\r");
        } else {
            p("File download failed\n\r");
        }

    } else {
        Serial.println("Failed to open remote file");
    }
}

void loop(){}

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