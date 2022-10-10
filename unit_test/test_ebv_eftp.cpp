#include "test_ebv_eftp.h"
#include "mock/mock_ebv_i2c.h"
#include "ebv_unit_compare.h"

#include <string.h>

void test_ebv_eftp_open(){
    {
        // Testing file open request with all valid data
        // Setup the test
        mock_i2c_init();
        const unsigned char i2c_resp_ack[] = MOCK_I2C_RESPONSE_ACK_READ_LOCAL_FILE;
        const unsigned char i2c_resp_delayed[] = MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_OPEN_WRITE_OK;
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response(i2c_resp_delayed, sizeof(i2c_resp_delayed));

        // Test
        bool ret = ebv_eftp_open("file", "w");
        TEST_EQUAL(ret, true);
    }
    {
        // Testing file open request failed by invalid req
        // Setup the test
        mock_i2c_init();
        const unsigned char i2c_resp_ack[] = MOCK_I2C_RESPONSE_ACK_READ_LOCAL_FILE;
        const unsigned char i2c_resp_delayed[] = MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_FAIL(_MOCK_I2C_ESP_ERROR_CODE_INVALID_CMD_DATA);
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response(i2c_resp_delayed, sizeof(i2c_resp_delayed));

        // Test
        bool ret = ebv_eftp_open("file", "w");
        TEST_EQUAL(ret, false);
        int err_code = ebv_eftp_get_latest_error_code();
        TEST_EQUAL(EBV_ESP_REMOTE_FILE_ERROR_INVALID_REQUEST, err_code);

        
    }
    {
        // Testing file open request failed by eftp error
        // Setup the test
        mock_i2c_init();
        const unsigned char i2c_resp_ack[] = MOCK_I2C_RESPONSE_ACK_READ_LOCAL_FILE;
        const unsigned char i2c_resp_delayed[] = MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_FAIL(_MOCK_I2C_ESP_ERROR_CODE_INTERNAL_ERROR);
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response(i2c_resp_delayed, sizeof(i2c_resp_delayed));

        // Test
        bool ret = ebv_eftp_open("file", "w");
        TEST_EQUAL(ret, false);
        int err_code = ebv_eftp_get_latest_error_code();
        TEST_EQUAL(EBV_ESP_REMOTE_FILE_ERROR_OPEN_FAILED, err_code);
    }
}

void test_ebv_eftp_write(){
    {
        // Testing file write OK
        // Setup the test
        mock_i2c_init();
        const unsigned char i2c_resp_ack[] = MOCK_I2C_RESPONSE_ACK_READ_LOCAL_FILE;
        const unsigned char i2c_resp_delayed[] = MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_WRITE_OK;
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response(i2c_resp_delayed, sizeof(i2c_resp_delayed));

        // Test
        char dummy_data[] = {0x00};
        bool ret = ebv_eftp_write(dummy_data, sizeof(dummy_data));
        TEST_EQUAL(ret, true);
    }
    {
        // Testing file write failed due to RESOURCE BUSY
        // Setup the test
        mock_i2c_init();
        const unsigned char i2c_resp_ack[] = MOCK_I2C_RESPONSE_ACK_READ_LOCAL_FILE;
        const unsigned char i2c_resp_delayed[] = MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_FAIL(_MOCK_I2C_ESP_ERROR_CODE_RESOURCE_BUSY);
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response(i2c_resp_delayed, sizeof(i2c_resp_delayed));

        // Test
        char dummy_data[] = {0x00};
        bool ret = ebv_eftp_write(dummy_data, sizeof(dummy_data));
        TEST_EQUAL(ret, false);
        int err_code = ebv_eftp_get_latest_error_code();
        TEST_EQUAL(EBV_ESP_REMOTE_FILE_ERROR_RESOURCE_BUSY, err_code);
    }
    {
        // Testing file write ok, fail, and ok again
        // Setup the test
        mock_i2c_init();
        const unsigned char i2c_resp_ack[] = MOCK_I2C_RESPONSE_ACK_READ_LOCAL_FILE;
        const unsigned char i2c_resp_delayed_fail[] = MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_FAIL(_MOCK_I2C_ESP_ERROR_CODE_RESOURCE_BUSY);
        const unsigned char i2c_resp_delayed_ok[] = MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_WRITE_OK;
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response(i2c_resp_delayed_ok, sizeof(i2c_resp_delayed_ok));
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response(i2c_resp_delayed_fail, sizeof(i2c_resp_delayed_fail));
        mock_ebv_i2c_set_response(i2c_resp_ack, sizeof(i2c_resp_ack));
        mock_ebv_i2c_set_delayed_response(i2c_resp_delayed_ok, sizeof(i2c_resp_delayed_ok));

        // Test
        char dummy_data[] = {0x00};
        bool ret_ok_1   = ebv_eftp_write(dummy_data, sizeof(dummy_data));
        bool ret_fail_1 = ebv_eftp_write(dummy_data, sizeof(dummy_data));
        bool ret_ok_2   = ebv_eftp_write(dummy_data, sizeof(dummy_data));
        TEST_EQUAL(ret_ok_1 && !ret_fail_1 && ret_ok_2, true);
        int err_code = ebv_eftp_get_latest_error_code();
        TEST_EQUAL(EBV_ESP_REMOTE_FILE_ERROR_RESOURCE_BUSY, err_code);
    }
}