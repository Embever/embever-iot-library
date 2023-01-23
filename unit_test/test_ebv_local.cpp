#include "test_ebv_local.h"
#include "ebv_unit_compare.h"
#include "mock/mock_ebv_i2c.h"

#include "ebv_esp.h"

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