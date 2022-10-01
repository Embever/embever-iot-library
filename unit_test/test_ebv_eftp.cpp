#include "test_ebv_eftp.h"
#include "mock/mock_ebv_i2c.h"
#include "ebv_unit_compare.h"

#include <string.h>

void test_ebv_eftp_open(){
    {
        // Testing file open request with all valid data
        // Setup the test
        mock_i2c_init();
        const unsigned char i2c_resp_1[] = MOCK_I2C_RESPONSE_ACK_READ_LOCAL_FILE;
        const unsigned char i2c_resp_2[] = MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_OPEN_WRITE_OK;
        const unsigned char i2c_resp_3[] = MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_OPEN_WRITE_OK;
        mock_ebv_i2c_set_response(0, i2c_resp_1, sizeof(i2c_resp_1));
        mock_ebv_i2c_set_response(1, i2c_resp_2, sizeof(i2c_resp_2));
        mock_ebv_i2c_set_response(2, i2c_resp_3, sizeof(i2c_resp_3));

        // Test
        bool ret = ebv_eftp_open("file", "w");
        TEST_EQUAL(ret, true);
    }
    {
        // Testing file open request failed
        // Setup the test
        mock_i2c_init();
        const unsigned char i2c_resp_1[] = MOCK_I2C_RESPONSE_ACK_READ_LOCAL_FILE;
        const unsigned char i2c_resp_2[] = MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_OPEN_WRITE_FAIL;
        const unsigned char i2c_resp_3[] = MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_OPEN_WRITE_FAIL;
        mock_ebv_i2c_set_response(0, i2c_resp_1, sizeof(i2c_resp_1));
        mock_ebv_i2c_set_response(1, i2c_resp_2, sizeof(i2c_resp_2));
        mock_ebv_i2c_set_response(2, i2c_resp_3, sizeof(i2c_resp_3));

        // Test
        bool ret = ebv_eftp_open("file", "w");
        TEST_EQUAL(ret, false);
        int err_code = ebv_eftp_get_latest_error_code();
        TEST_EQUAL(EBV_ESP_REMOTE_FILE_ERROR_RESOURCE_BUSY, err_code);
    }
}