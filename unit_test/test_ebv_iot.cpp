#include "test_ebv_iot.h"
#include "ebv_unit_compare.h"

UNIT_TEST_SETUP_EBV_IOT_CB;

void test_ebv_iot_list_builder_by_array(){
    UNIT_TEST_REGISTER_ARDUINO_CB;
    const uint8_t a = 1, b = 2, c = 3, d = 4, e = 5;        // constant variables for demonstrating
    EBV_IOT_BUILD_UINT_LIST(uint_list, a, b, c, d, e);      // build the integer list list into the uint_list variable
    if(uint_list.buf_len == 0){                             // Check the result, the buf_len should greater that 0
        return;
    }
    // TEST
    {
        uint8_t expected_data[] = {0x95, 0x01, 0x02, 0x03, 0x04, 0x05};
        TEST_EQUAL(sizeof(expected_data), uint_list.buf_len);
        TEST_ARR_EQ(expected_data, uint_list.buf, sizeof(expected_data));
    }

    uint16_t my_data_array[] = {10, 9, 8, 7, 6};
    EBV_IOT_BUILD_UINT_LIST_BY_ARRAY(array_list, my_data_array, ARR_SIZE(my_data_array));

    // TEST
    {
        uint8_t expected_data[] = {0x95, 0x0A, 0x09, 0x08, 0x07, 0x06};
        TEST_EQUAL(sizeof(expected_data), array_list.buf_len);
        TEST_ARR_EQ(expected_data, array_list.buf, sizeof(expected_data));
    }

    ebv_iot_initGenericEvent("Hi_Cloud");                           // Set the event type
    ebv_iot_addGenericPayload("key", "value");                      // Add payload
    ebv_iot_addGenericPayload("uint_list", &uint_list);             // Add the integer list payload
    ebv_iot_addGenericPayload("my_data_array", &array_list);        // Add the integer list payload
    ebv_iot_addGenericPayload("k", "v");                            // Add more data
    ebv_iot_submitGenericEvent();                                   // Send event

    // TEST
    {
        extern ebv_mpack _ebv_mpack;
        // https://kawanet.github.io/msgpack-lite/
        uint8_t expected[] = {
            0x91, 0x92, 0xa8, 0x48, 0x69, 0x5f, 0x43, 0x6c, 0x6f, 0x75, 0x64, 0x84, 0xa3, 0x6b, 0x65, 0x79,
            0xa5, 0x76, 0x61, 0x6c, 0x75, 0x65, 0xa9, 0x75, 0x69, 0x6e, 0x74, 0x5f, 0x6c, 0x69, 0x73, 0x74,
            0x95, 0x01, 0x02, 0x03, 0x04, 0x05, 0xad, 0x6d, 0x79, 0x5f, 0x64, 0x61, 0x74, 0x61, 0x5f, 0x61,
            0x72, 0x72, 0x61, 0x79, 0x95, 0x0a, 0x09, 0x08, 0x07, 0x06, 0xa1, 0x6b, 0xa1, 0x76
        };
        TEST_EQUAL(sizeof(expected), (_ebv_mpack.c.current - _ebv_mpack.c.start));
        TEST_ARR_EQ(expected, _ebv_mpack.buff, sizeof(expected));
    }
    return;
}