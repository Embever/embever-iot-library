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

// SD File List
// CAAM.pdf                  70 KB
// ESP.pdf                  383 KB
// IMG.jpg                  1,7 MB
// nRF.pdf                  6,8 MB
// XBEE3.pdf                4,5 MB

#include "ebv_iot.h"
#include "print_serial.h"
#include "Wire.h"
#include "ebv_boards.h"

#define FILE_DATA_TRANSMISSION_FRAME_LEN   (IOT_MSG_MAX_LEN - ESP_CMD_PACKET_OVERHEAD)  // The maximum payload size of an esp packet 

#define USE_STATIC_DATA                 0                           // Use a preset data array for the upload
#define GENERATE_FILE_CONTENT           0                           // Use a generated ( in runtime) data array for the upload
#define GENERATED_FILE_CONTENT_LENGTH   4 * 1024                    // The array size of the generated data
#define REMOTE_FILE_NAME                "app_mcu_file_1"            // The name of the uploaded file in the cloud (in case of STATIC_DATA nad GENERATED_DATA)
#define USE_SD_CARD                     1                           // Use a file from an SD card as data source for the upload
#define SD_FILE_NAME                    "CAAM.pdf"                   // The name of the file which will be uploaded
// #define SD_FILE_NAME                    "nucleo.pdf"             // The name of the file which will be uploaded

// Guard to not let use multiple data sources at the same time
#if USE_SD_CARD + GENERATE_FILE_CONTENT + USE_STATIC_DATA > 1
#error "Only one feature can be enabled"
#endif
#if USE_SD_CARD + GENERATE_FILE_CONTENT + USE_STATIC_DATA == 0
#error "One of feature must be enabled"
#endif

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
    Serial.println("Press to start the file upload...");
    while(digitalRead(PIN_BTN));
    if( ebv_util_wait_device_ready(DEFAULT_NETWORK_ATTACH_TIMEOUT_SEC) == false){
        p("Device is not ready, timeout reached\n\r");
        print_fail_reason();
        while(1);
    }
    Serial.println("Starting file upload");
#if USE_SD_CARD == 1
    bool ret = ebv_eftp_open(SD_FILE_NAME, "w");
#else
    bool ret = ebv_eftp_open(REMOTE_FILE_NAME, "w");
#endif
    if(ret){
#if USE_STATIC_DATA == 1
        char file_data[] = "This file came from the APP mcu over esp";
        const unsigned int file_len = sizeof(file_data);
#endif
#if GENERATE_FILE_CONTENT == 1
        char file_data[GENERATED_FILE_CONTENT_LENGTH];
        const int file_len = sizeof(file_data);
        for(int index = 0; index < file_len; index++){
            file_data[index] = index % 256;
        }
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
    p("File size on SD Card: %d\n\r", file_len);
#endif
        int index = 0;
        int write_len;
        do {
#if USE_SD_CARD == 1
            write_len = mFile.read(file_data, sizeof(file_data));
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
                    esp_err_t err =  ebv_iot_get_last_error_code();
                    p("Received error code %d\r\n", err);
                    if(err == ESP_ERR_RESOURCE_BUSY){
                        // Need to wait a bit, the file buffer is full on the CaaM side
                        Serial.println("Resource busy, retrying...");
                        delay(1000);
                    } else {
                        // Something else
                        p("Unknown error during file transfer: %d, aborting\n\r", err);
                        write_len = 0;
                        break;
                    }
                }
            } while(!ret);
        } while(write_len == sizeof(file_data));
        
        ebv_eftp_close();
        Serial.println("File transfer DONE");
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