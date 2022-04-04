#include <Arduino.h>
#include <HardwareSerial.h>

// Usefull definitions
#define ARDUINO_AVR_PIN_A0 14
#define ARDUINO_AVR_PIN_A1 15
#define ARDUINO_AVR_PIN_A2 16
#define ARDUINO_AVR_PIN_A3 17

// Define the following macros to assign custom GPIO pins for READY and IRQ lines
// The default configuration is the following:
// Arduino pin A3 -- ESP READY
// Arduino pin A2 -- ESP IRQ
// #define PIN_EBV_IRQ     ARDUINO_AVR_PIN_A0       // ESP IRQ signal connected here
// #define PIN_EBV_READY   ARDUINO_AVR_PIN_A1       // ESP IRQ signal connected here

#define PIN_FETCH_BTN       14

#include "ebv_iot.h"
#include "print_serial.h"
#include "Wire/EBV_Wire.h"

EBV_SETUP_ARDUINO_CB;
LOG_SETUP_ARDUINO;

void setup() {
    Serial.begin(115200);
    EBV_REGISTER_ARDUINO_CB;
    LOG_REGISTER_ARDUINO;
    p("\n\rSimple GNSS sample starting...\n\r");
}

void loop(){
    while( digitalRead(PIN_FETCH_BTN) );
    ebv_gnss_data_t gnss_data;
    bool ret = ebv_local_query_gnss(&gnss_data);
    if(ret){
        if(gnss_data.has_fix){
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
            p("Date: %d:%d:%d Time: %d:%d:%d\n\r",
                gnss_data.datetime.day,
                gnss_data.datetime.month,
                gnss_data.datetime.year,
                gnss_data.datetime.hour,
                gnss_data.datetime.minute,
                gnss_data.datetime.seconds
            );
        } else {
            p("GPS position not available\n\r");
        }
    } else {
        p("Failed to receive GNSS data\n\r");
    }
    p("Query SPEED and DATE_TIME only\n\r");
    memset(&gnss_data, 0, sizeof(ebv_gnss_data_t));
    ebv_local_query_gnss_custom_init();
    ebv_local_query_gnss_custom_add(EBV_GNSS_REQUEST_SPEED);
    ebv_local_query_gnss_custom_add(EBV_GNSS_REQUEST_DATETIME);
    if( ebv_local_query_gnss_custom_add_submit(&gnss_data) ){
        p("Speed: %f, Heading: %f\n\r",
                gnss_data.speed,
                gnss_data.heading
        );
        p("Date: %d:%d:%d Time: %d:%d:%d\n\r",
                gnss_data.datetime.day,
                gnss_data.datetime.month,
                gnss_data.datetime.year,
                gnss_data.datetime.hour,
                gnss_data.datetime.minute,
                gnss_data.datetime.seconds
        );
    }

    delay(2000);
}