#ifndef INC_EBV_LOCAL_H
#define INC_EBV_LOCAL_H

#include <Arduino.h>

#ifdef EBV_UNIT_TEST
#include "ebv_esp.h"
#endif

typedef enum {
    EBV_GNSS_REQUEST_LOCATION = 0,
    EBV_GNSS_REQUEST_SPEED,
    EBV_GNSS_REQUEST_DATETIME,
    EBV_GNSS_REQUEST_EXTENDED,
    EBV_GNSS_REPORT_LOCATION,
    EBV_GNSS_REPORT_DATETIME,
    EBV_GNSS_REQUEST_STATUS,
    EBV_GNSS_REQUEST_LEN
} ebv_gnss_request_kind;

typedef enum {
    EBV_GPS_STATUS_UNKNOWN = 0,
    EBV_GPS_STATUS_INITIALIZED,
    EBV_GPS_STATUS_SEARCHING_SINGLE,
    EBV_GPS_STATUS_SEARCHING_CONT,
    EBV_GPS_STATUS_TRACKING,
    EBV_GPS_STATUS_FIXED,
    EBV_GPS_STATUS_STOPPED
} EBV_GPS_STATUS_t;

typedef struct{
    uint8_t state;
    bool is_last_fix_success;
} ebv_gps_status_t;

typedef struct {
    uint16_t year;                 // 4-digit representation (Gregorian calendar)
    uint8_t month;                 // 1...12
    uint8_t day;                   // 1...31
    uint8_t hour;                  // 0...23
    uint8_t minute;                // 0...59
    uint8_t seconds;               // 0...59
    uint16_t ms;                   // 0...999
} esp_gnss_datetime_t;

typedef struct{
    double lat;
    double lon;
    float altitude;
    float accuracy;
    float speed;
    float heading;
    bool has_fix;
    esp_gnss_datetime_t datetime;
} ebv_gnss_data_t;

typedef enum{
    EBV_OP_MODE_ONLINE,
    EBV_OP_MODE_PWR_DOWN
} ebv_local_pwr_op_mode;


bool ebv_local_query_gnss(ebv_gnss_data_t *pvt);
bool ebv_report_pvt();
bool ebv_query_gps_status(ebv_gps_status_t *status);
void ebv_local_query_gnss_custom_init();
bool ebv_local_query_gnss_custom_add(ebv_gnss_request_kind k );
bool ebv_local_query_gnss_custom_add_submit(ebv_gnss_data_t *pvt);

bool ebv_local_set_op_mode(ebv_local_pwr_op_mode op_mode);

#ifdef EBV_UNIT_TEST
#warning "UNIT TEST ACTIVE"
#define ebv_unit_test_static
bool _ebv_local_parse_gnss_response(esp_response_t *response, ebv_gnss_data_t *pvt);
bool _ebv_local_verify_gnss_response(uint8_t *payload, uint8_t payload_len, ebv_gnss_request_kind query_type, ebv_gnss_data_t *pvt);
#else
#define ebv_unit_test_static static
#endif

#endif