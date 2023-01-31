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

enum ebv_gnss_nav_mode{
    EBV_GNSS_NAV_MODE_SINGLE = 0,
    EBV_GNSS_NAV_MODE_CONTINUOUS,
    EBV_GNSS_NAV_MODE_COUNT
};

enum ebv_modem_rf_mode {
    EBV_MODEM_RF_MODE_NBIOT,
    EBV_MODEM_RF_MODE_LTEM,
    EBV_MODEM_RF_MODE_GPS_ONLY,
    EBV_MODEM_RF_MODE_OFFLINE,
    EBV_MODEM_RF_MODE_RADIO_TEST,
    EBV_MODEM_RF_MODE_COUNT,
    EBV_MODEM_RF_MODE_INVALID
};

enum ebv_modem_lte_mode{
    EBV_MODEM_LTE_MODE_NB_IOT,
    EBV_MODEM_LTE_MODE_LTEM,
    EBV_MODEM_LTE_MODE_NONE,
    EBV_MODEM_LTE_MODE_UNKNOWN,
    EBV_MODEM_LTE_MODE_COUNT,
    EBV_MODEM_LTE_MODE_INVALID
};

enum ebv_modem_network_status{
    EBV_MODEM_NETWORK_STATUS_INITIALISED,
    EBV_MODEM_NETWORK_STATUS_SEARCHING,
    EBV_MODEM_NETWORK_STATUS_REGISTERED_HOME,
    EBV_MODEM_NETWORK_STATUS_REGISTERED_ROAMING,
    EBV_MODEM_NETWORK_STATUS_REGISTRATION_FAILED,
    EBV_MODEM_NETWORK_STATUS_UNKNOWN,
    EBV_MODEM_NETWORK_STATUS_COUNT,
    EBV_MODEM_NETWORK_STATUS_INVALID
};

#define  EBV_GNSS_REQUEST_OPTIONAL_PARAM_TIMEOUT        "timeout"
#define  EBV_GNSS_REQUEST_OPTIONAL_PARAM_MIN_ACCURACY   "accuracy"
#define  EBV_GNSS_REQUEST_OPTIONAL_PARAM_NAV_MODE       "nav_mode"

#define EBV_STATUS_GROUP_MODEM_KEY                      "modem"
#define EBV_STATUS_MODEM_RF_MODE_KEY                    "rf_mode"
#define EBV_STATUS_MODEM_LTE_STATUS_KEY                 "lte_mode"
#define EBV_STATUS_MODEM_NET_STATUS_KEY                 "net_status"



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
    ebv_gps_status_t status;
} ebv_gnss_data_t;

typedef enum{
    EBV_OP_MODE_ONLINE,
    EBV_OP_MODE_PWR_DOWN
} ebv_local_pwr_op_mode;

typedef struct{
    enum ebv_modem_rf_mode rf_mode;
    enum ebv_modem_lte_mode lte_mode;
    enum ebv_modem_network_status network_status;
} ebv_local_modem_status_t;


bool ebv_local_query_gnss(ebv_gnss_data_t *pvt);
bool ebv_local_query_gnss_cont(ebv_gnss_data_t *pvt);
bool ebv_report_pvt();
bool ebv_report_pvt_custom_params(uint8_t max_timeout, uint8_t min_acc);
bool ebv_query_gps_status(ebv_gps_status_t *status);
void ebv_local_query_gnss_custom_init();
bool ebv_local_query_gnss_custom_add(ebv_gnss_request_kind k );
bool ebv_local_query_gnss_custom_add_submit(ebv_gnss_data_t *pvt);
bool ebv_local_set_op_mode(ebv_local_pwr_op_mode op_mode);
bool ebv_local_set_rf_mode(enum ebv_modem_rf_mode rf_mode);
bool ebv_local_status_update_modem(ebv_local_modem_status_t * status);
void ebv_local_status_modem_str(ebv_local_modem_status_t *status, char *lte_mode_str, char *network_status_str, char *rf_mode_str);

#ifdef EBV_UNIT_TEST
#warning "UNIT TEST ACTIVE"
#define ebv_unit_test_static
bool _ebv_local_parse_gnss_response(esp_response_t *response, ebv_gnss_data_t *pvt);
bool _ebv_local_verify_gnss_response(uint8_t *payload, uint8_t payload_len, ebv_gnss_request_kind query_type, ebv_gnss_data_t *pvt);
#else
#define ebv_unit_test_static static
#endif

#endif