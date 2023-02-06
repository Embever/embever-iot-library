#include "test_ebv_local.h"
#include "ebv_unit_compare.h"
#include "mock/mock_ebv_i2c.h"
#include "util/test_esp_util.h"

#include "ebv_esp.h"
#include "ebv_local.h"

void test_ebv_local_verify_gnss_response(){
    ebv_gnss_data_t pvt;
    // Extended GPS data [[LOCATION] [SPEED] [[DATE][TIME]]]
    uint8_t payload_1[] = {
        0x93, 0x93, 0x92, 0xcb, 
        0x40, 0x4a,  0xe, 0xef, 0xdf, 0xf7, 0x4b, 0xfc, 
        0xcb, 0x40, 0x27, 0x3f, 0xe5, 0x75, 0x3a, 0x3e, 
        0xc0, 0xca, 0x42, 0xbf,  0x0,  0x0, 0xca, 0x40, 
        0xa0, 0x0 ,  0x0, 0x92, 0xca, 0x41, 0xf0, 0x0 ,
        0x0 , 0xca ,0x42, 0xb4,  0x0,  0x0, 0x92, 0x93, 
        0xcd, 0x7,  0xe5,  0x5, 0x19, 0x93,  0xa, 0x3b ,
        0x1e
    };
    ebv_gnss_data_t expected_pvt_1 = (ebv_gnss_data_t) {
        .lat = 52.1166954,
        .lon = 11.6247975,
        .altitude = 95.5,
        .accuracy = 5,
        .speed = 30,
        .heading = 90,
        .datetime.day = 25,
        .datetime.month = 5,
        .datetime.year = 2021,
        .datetime.hour = 10,
        .datetime.minute = 59,
        .datetime.seconds = 30
    };
    _ebv_local_verify_gnss_response(payload_1, sizeof(payload_1), EBV_GNSS_REQUEST_EXTENDED, &pvt);
   TEST_EQUAL(expected_pvt_1.lat,               pvt.lat);
   TEST_EQUAL(expected_pvt_1.lon,               pvt.lon);
   TEST_EQUAL(expected_pvt_1.altitude,          pvt.altitude);
   TEST_EQUAL(expected_pvt_1.accuracy,          pvt.accuracy);
   TEST_EQUAL(expected_pvt_1.speed,             pvt.speed);
   TEST_EQUAL(expected_pvt_1.heading,           pvt.heading);
   TEST_EQUAL(expected_pvt_1.datetime.year,     pvt.datetime.year);
   TEST_EQUAL(expected_pvt_1.datetime.month,    pvt.datetime.month);
   TEST_EQUAL(expected_pvt_1.datetime.day,      pvt.datetime.day);
   TEST_EQUAL(expected_pvt_1.datetime.hour,     pvt.datetime.hour);
   TEST_EQUAL(expected_pvt_1.datetime.minute,   pvt.datetime.minute);
   TEST_EQUAL(expected_pvt_1.datetime.seconds,  pvt.datetime.seconds);
}

void test_ebv_local_parse_gnss_response(){
    extern uint8_t _gnss_query_data[32];
    extern uint8_t _gnss_query_data_len;
    extern uint8_t _gnss_nof_queries;
    extern uint8_t *_gnss_queries;
    _gnss_query_data[0] = 0x91;
    _gnss_query_data[1] = 0x91;
    _gnss_query_data[2] = EBV_GNSS_REQUEST_SPEED;
    _gnss_nof_queries = 1;
    _gnss_queries = & (_gnss_query_data[2]);
    _gnss_query_data_len = 3;
    ebv_gnss_data_t pvt;
    esp_response_t response;
    uint8_t payload_1[] = {
        0x91, 0x92, 0xca, 0x41,
        0xf0,  0x0,  0x0, 0xca, 0x42, 0xb4, 0x0, 0x0 
    };
    memcpy(response.response, payload_1, sizeof(payload_1));
    response.payload = response.response;
    response.payload_len = sizeof(payload_1);
    _ebv_local_parse_gnss_response(&response, &pvt);
}

void test_ebv_local_gnss_custom(){

    {
        mock_i2c_init();
        const unsigned char i2c_resp_ack[] = MOCK_I2C_RESPONSE_ACK_READ_LOCAL_FILE;
        const unsigned char i2c_resp_delayed[] = MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_OPEN_WRITE_OK;
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response(i2c_resp_delayed, sizeof(i2c_resp_delayed));
        
        ebv_gnss_data_t gnss_data;
        ebv_local_query_gnss_custom_init();
        ebv_local_query_gnss_custom_add(EBV_GNSS_REQUEST_LOCATION);
        ebv_local_query_gnss_custom_add(EBV_GNSS_REQUEST_DATETIME);
        ebv_local_query_gnss_custom_add(EBV_GNSS_REQUEST_STATUS);
        bool ret = ebv_local_query_gnss_custom_add_submit(&gnss_data);
    }
    {
        extern uint8_t _gnss_query_data[32];
        extern uint8_t _gnss_query_data_len;
        extern uint8_t _gnss_nof_queries;
        extern uint8_t *_gnss_queries;
        ebv_gnss_data_t pvt;
        ebv_local_query_gnss_cont(&pvt);
    }
}

void test_local_set_rf_mode(){
    // DELAYED RESPONSE OK
    {
        mock_i2c_init();
        const unsigned char i2c_resp_ack[] = MOCK_I2C_RESPONSE_ACK_CONFIG;
        const unsigned char i2c_resp_delayed[] = MOCK_I2C_DELAYED_RESPONSE_CONFIG_OK;
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response(i2c_resp_delayed, sizeof(i2c_resp_delayed));

        bool ret = ebv_local_set_rf_mode(EBV_MODEM_RF_MODE_NBIOT);
        TEST_EQUAL(ret, true);
    }
    // DELAYED RESPONSE FAIL
    {
        mock_i2c_init();
        const unsigned char i2c_resp_ack[] = MOCK_I2C_RESPONSE_ACK_CONFIG;
        const unsigned char i2c_resp_delayed[] = MOCK_I2C_DELAYED_RESPONSE_CONFIG_FAIL(_MOCK_I2C_ESP_ERROR_CODE_INVALID_CMD_DATA);
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response(i2c_resp_delayed, sizeof(i2c_resp_delayed));

        bool ret = ebv_local_set_rf_mode(EBV_MODEM_RF_MODE_NBIOT);
        TEST_EQUAL(ret, true);
    }
}

void test_ebv_local_status_update_modem(){
    {
        mock_i2c_init();
        const unsigned char i2c_resp_ack[] = MOCK_I2C_RESPONSE_ACK_STATUS;
        // [[  "modem",  {    "rf_mode": 0,    "lte_mode": 0,    "net_status": 0  }]]
        const char esp_delayed_payload[] = {0x91, 0x92, 0xA5, 0x6D, 0x6F, 0x64, 0x65, 0x6D, 0x83, 0xA7, 0x72, 0x66, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x00, 0xA8, 0x6C, 0x74, 0x65, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x00, 0xAA, 0x6E, 0x65, 0x74, 0x5F, 0x73, 0x74, 0x61, 0x74, 0x75, 0x73, 0x00};
        char esp_pkg[512];
        uint16_t esp_pkg_len = test_esp_util_build_delayed_response(esp_pkg, ESP_CMD_STATUS, esp_delayed_payload, sizeof(esp_delayed_payload));
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response((const unsigned char *) esp_pkg, esp_pkg_len);

        ebv_local_modem_status_t status;
        bool ret = ebv_local_status_update_modem(&status);

        TEST_EQUAL(ret, true);
        TEST_EQUAL(status.lte_mode, EBV_MODEM_LTE_MODE_NB_IOT);
        TEST_EQUAL(status.network_status, EBV_MODEM_NETWORK_STATUS_INITIALISED);
        TEST_EQUAL(status.rf_mode, EBV_MODEM_RF_MODE_NBIOT);
    }
    {
        mock_i2c_init();
        const unsigned char i2c_resp_ack[] = MOCK_I2C_RESPONSE_ACK_STATUS;
        // [[  "modem",  {    "rf_mode": 1,    "lte_mode": 2,    "net_status": 3  }]]
        const char esp_delayed_payload[] = {0x91, 0x92, 0xA5, 0x6D, 0x6F, 0x64, 0x65, 0x6D, 0x83, 0xA7, 0x72, 0x66, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x01, 0xA8, 0x6C, 0x74, 0x65, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x02, 0xAA, 0x6E, 0x65, 0x74, 0x5F, 0x73, 0x74, 0x61, 0x74, 0x75, 0x73, 0x03};
        char esp_pkg[512];
        uint16_t esp_pkg_len = test_esp_util_build_delayed_response(esp_pkg, ESP_CMD_STATUS, esp_delayed_payload, sizeof(esp_delayed_payload));
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response((const unsigned char *) esp_pkg, esp_pkg_len);

        ebv_local_modem_status_t status;
        bool ret = ebv_local_status_update_modem(&status);

        TEST_EQUAL(ret, true);
        TEST_EQUAL(status.lte_mode, EBV_MODEM_LTE_MODE_NONE);
        TEST_EQUAL(status.network_status, EBV_MODEM_NETWORK_STATUS_REGISTERED_ROAMING);
        TEST_EQUAL(status.rf_mode, EBV_MODEM_RF_MODE_LTEM);
    }
    {
        mock_i2c_init();
        const unsigned char i2c_resp_ack[] = MOCK_I2C_RESPONSE_ACK_STATUS;
        // [[  "modem",  {    "rf_mode": 1,    "lte_mode": 2,    "net_status": 3  }]]
        const char esp_delayed_payload[] = {0x91, 0x92, 0xA5, 0x6D, 0x6F, 0x64, 0x65, 0x6D, 0x83, 0xAA, 0x6E, 0x65, 0x74, 0x5F, 0x73, 0x74, 0x61, 0x74, 0x75, 0x73, 0x03, 0xA7, 0x72, 0x66, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x01, 0xA8, 0x6C, 0x74, 0x65, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x02};
        char esp_pkg[512];
        uint16_t esp_pkg_len = test_esp_util_build_delayed_response(esp_pkg, ESP_CMD_STATUS, esp_delayed_payload, sizeof(esp_delayed_payload));
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response((const unsigned char *) esp_pkg, esp_pkg_len);

        ebv_local_modem_status_t status;
        bool ret = ebv_local_status_update_modem(&status);

        TEST_EQUAL(ret, true);
        TEST_EQUAL(status.lte_mode, EBV_MODEM_LTE_MODE_NONE);
        TEST_EQUAL(status.network_status, EBV_MODEM_NETWORK_STATUS_REGISTERED_ROAMING);
        TEST_EQUAL(status.rf_mode, EBV_MODEM_RF_MODE_LTEM);
    }
}