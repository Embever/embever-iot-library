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



void setup() {
    Serial.begin(115200);
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\n\rSimple GNSS sample starting\r\n");
    // p("Press the button to start GPS acquisitions\r\n");
    // while( digitalRead(PIN_BTN) );
}

void loop(){
  
    ebv_gnss_data_t gnss_data;
    bool ret = ebv_local_query_gnss(&gnss_data);
    if(ret){
        if(gnss_data.has_fix){
            p("\n\rGNSS data:\n\r");
            p("Lat: %lf, Lon: %lf, ALT: %f, ACC: %f\n\r",
                gnss_data.lat,
                gnss_data.lon,
                gnss_data.altitude,
                gnss_data.accuracy
            );
            p("Speed: %f, Heading: %f\n\r",
                gnss_data.speed,
                gnss_data.heading
            );
            p("Date: %02d:%02d:%d Time: %02d:%02d:%02d\n\r",
                gnss_data.datetime.day,
                gnss_data.datetime.month,
                gnss_data.datetime.year,
                gnss_data.datetime.hour,
                gnss_data.datetime.minute,
                gnss_data.datetime.seconds
            );
        } else {
            p("GPS position not available\n\r");
            delay(5 * 1000);
            return;
        }
    } else {
        p("Failed to receive GNSS data\n\r");
    }
    delay(5 * 1000);
    p("\n\rQuery SPEED and DATE_TIME only\n\r");
    memset(&gnss_data, 0, sizeof(ebv_gnss_data_t));
    ebv_local_query_gnss_custom_init();
    ebv_local_query_gnss_custom_add(EBV_GNSS_REQUEST_SPEED);
    ebv_local_query_gnss_custom_add(EBV_GNSS_REQUEST_DATETIME);
    if( ebv_local_query_gnss_custom_add_submit(&gnss_data) ){
        p("Speed: %f, Heading: %f\n\r",
                gnss_data.speed,
                gnss_data.heading
        );
        p("Date: %02d:%02d:%d Time: %02d:%02d:%02d\n\r",
                gnss_data.datetime.day,
                gnss_data.datetime.month,
                gnss_data.datetime.year,
                gnss_data.datetime.hour,
                gnss_data.datetime.minute,
                gnss_data.datetime.seconds
        );
    }

    delay(10 * 1000);
}