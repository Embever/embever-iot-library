#include <Arduino.h>
#include <HardwareSerial.h>
#include <extcwpack.h>

// Define the following macros to assign custom GPIO pins for each functions
// The default configuration is the following:
// Arduino pin A3 -- PIN_ESP READY
// Arduino pin A2 -- PIN_ESP_IRQ
// Arduino pin  5 -- PIN_BTN
// Arduino pin  4 -- PIN_BTN

#define PIN_EBV_IRQ                       11       // ESP IRQ signal connected here
#define PIN_EBV_READY                     10       // ESP READY signal connected here
#define PIN_BTN                            2       // Push button, with pullup resistor, the btn pulling the signal low
#define PIN_LED                           A1       // LED, active LOW

#include "ebv_iot.h"
#include "print_serial.h"
#include "Wire.h"
#include "ebv_boards.h"

#include "mcp9808.h"

#define SECOND 1000UL
#define MINUTE (SECOND * 60UL)
#define HOUR (MINUTE * 60UL)

EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void iot_send_temperature(double temp){
    ebv_iot_initGenericEvent("Temperature");
    _ebv_iot_addDoublePayload("temp_C", temp);
    bool ret = ebv_iot_submitGenericEvent();
    if(ret){
        p("Temperature Event sent\r\n");
    } else {
        p("Failed to send temperature event\r\n");
    }
}

void setup() {
    Serial.begin(115200);  // start serial for output
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\r\nEnvironment Monitor\r\n");
    if(mcp9808_init() == false){
        p("Failed to init temperature sensor\r\n");
    }
   
}

void loop(){
    float temp = mcp9808_get_temperature();
    uint8_t temp_upper = temp;
    uint8_t temp_lower = (float) ((temp - (float) temp_upper)) * 10.0;
    p("Temperature : %d.%d\r\n", temp_upper, temp_lower);
    double temperature = temp_upper + (((double) temp_lower) / 10.0);
    iot_send_temperature(temperature);
    delay( 15 * MINUTE);
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