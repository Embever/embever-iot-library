#include "test_ebv_iot.h"

UNIT_TEST_SETUP_EBV_IOT_CB;

void test_ebv_iot_list_builder_by_array(){
    UNIT_TEST_REGISTER_ARDUINO_CB;
    const uint8_t a = 1, b = 2, c = 3, d = 4, e = 5;        // constant variables for demonstrating
    EBV_IOT_BUILD_UINT_LIST(uint_list, a, b, c, d, e);      // build the integer list list into the uint_list variable
    if(uint_list.buf_len == 0){                             // Check the result, the buf_len should greater that 0
        return;
    }

    uint16_t my_data_array[] = {10, 9, 8, 7, 6};
    EBV_IOT_BUILD_UINT_LIST_BY_ARRAY(array_list, my_data_array, sizeof(my_data_array));

    ebv_iot_initGenericEvent("Hi_Cloud");                   // Set the event type
    ebv_iot_addGenericPayload("key", "value");                  // Add payload
    ebv_iot_addGenericPayload("uint_list", &uint_list);         // Add the integer list payload
    ebv_iot_addGenericPayload("my_data_array", &array_list);     // Add the integer list payload
    ebv_iot_addGenericPayload("k", "v");                        // Add more data
    bool ret = ebv_iot_submitGenericEvent();                    // Send event
    return;
}