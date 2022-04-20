#include "stdio.h"
#include "string.h"
#include "inttypes.h"
#include "string.h"
#include "test_ebv_local.h"

#include "ebv_unit_compare.h"

int test_passed = 0;
int test_failed = 0;


#define TEST_MSG_CACHE 0
#define TEST_MSG_VALIDATOR 1

// void slave_bank(){
//     slave_bank_init();
    
//     struct mac_addr_s mac;
//     mac.lsb = 0x11223344;
//     mac.msb = 0x44332211;
//     uint8_t slave_id = slave_bank_add(mac);
//     uint8_t slaves = slave_bank_getNumberOfSlaves();
//     TEST_EQUAL(slaves, 1);
//     bool ret = slave_bank_getMacById(slave_id, &mac);
//     TEST_EQUAL(ret, true);
//     TEST_EQUAL(mac.lsb, 0x11223344);
//     TEST_EQUAL(mac.msb, 0x44332211);

//     slave_id = slave_bank_add((struct mac_addr_s) {0x87654321, 0x12345678});
//     slaves = slave_bank_getNumberOfSlaves();
//     TEST_EQUAL(slaves, 2);
//     ret = slave_bank_getMacById(slave_id, &mac);
//     TEST_EQUAL(ret, true);
//     TEST_EQUAL(mac.lsb, 0x12345678);
//     TEST_EQUAL(mac.msb, 0x87654321);

//     slave_id = slave_bank_add((struct mac_addr_s) {0x20202020, 0x10101010});
//     slaves = slave_bank_getNumberOfSlaves();
//     TEST_EQUAL(slaves, 3);
//     ret = slave_bank_getMacById(slave_id, &mac);
//     TEST_EQUAL(ret, true);
//     TEST_EQUAL(mac.lsb, 0x10101010);
//     TEST_EQUAL(mac.msb, 0x20202020);

//     slave_id = slave_bank_add((struct mac_addr_s) {0x40404040, 0x30303030});
//     slaves = slave_bank_getNumberOfSlaves();
//     TEST_EQUAL(slaves, 4);
//     ret = slave_bank_getMacById(slave_id, &mac);
//     TEST_EQUAL(ret, true);
//     TEST_EQUAL(mac.lsb, 0x30303030);
//     TEST_EQUAL(mac.msb, 0x40404040);

//     // out of space here
//     slave_id = slave_bank_add((struct mac_addr_s) {0x60606060, 0x50505050});
//     slaves = slave_bank_getNumberOfSlaves();
//     TEST_EQUAL(slaves, 4);
//     ret = slave_bank_getMacById(slave_id, &mac);
//     TEST_EQUAL(ret, false);
//     TEST_EQUAL(mac.lsb, 0x00);
//     TEST_EQUAL(mac.msb, 0x00);

//     // Tests for setters and getters about moisture and valve data
//     struct moisture_s m;
//     m.a = 50;
//     m.b = 60;
//     ret = slave_bank_setMoistureData(0x01, m);
//     TEST_EQUAL(ret, true);
//     ret = slave_bank_getMoistureData(0x01, &m);
//     TEST_EQUAL(m.a, 50);
//     TEST_EQUAL(m.b, 60);
//     m.a = 10;
//     m.b = 20;
//     ret = slave_bank_setMoistureData(0x02, m);
//     TEST_EQUAL(ret, true);
//     m.a = 30;
//     m.b = 40;
//     ret = slave_bank_setMoistureData(0x03, m);
//     TEST_EQUAL(ret, true);
//     m.a = 70;
//     m.b = 80;
//     ret = slave_bank_setMoistureData(0x04, m);
//     TEST_EQUAL(ret, true);
//     m.a = 90;
//     m.b = 100;
//     ret = slave_bank_setMoistureData(0x05, m);
//     TEST_EQUAL(ret, false);
//     ret = slave_bank_getMoistureData(0x02, &m);
//     TEST_EQUAL(ret, true);
//     TEST_EQUAL(m.a, 10);
//     TEST_EQUAL(m.b, 20);
//     ret = slave_bank_getMoistureData(0x03, &m);
//     TEST_EQUAL(ret, true);
//     TEST_EQUAL(m.a, 30);
//     TEST_EQUAL(m.b, 40);
//     ret = slave_bank_getMoistureData(0x04, &m);
//     TEST_EQUAL(ret, true);
//     TEST_EQUAL(m.a, 70);
//     TEST_EQUAL(m.b, 80);
//     ret = slave_bank_getMoistureData(0x05, &m);
//     TEST_EQUAL(ret, false);
//     TEST_EQUAL(m.a, 0);
//     TEST_EQUAL(m.b, 0);

//     ret = slave_bank_getMoistureData(0x00, &m);
//     TEST_EQUAL(ret, false);
//     TEST_EQUAL(m.a, 0);
//     TEST_EQUAL(m.b, 0);

//     struct valves_s v;
//     v.a.state = VALVE_STATE_CLOSED;
//     v.a.timer = 0;
//     v.b.state = VALVE_STATE_OPEN;
//     v.b.timer = 10;
//     ret = slave_bank_setValveData(0x01, v);
//     TEST_EQUAL(ret, true);
//     v.a.state = VALVE_STATE_OPEN;
//     v.a.timer = 20;
//     v.b.state = VALVE_STATE_CLOSED;
//     v.b.timer = 30;
//     ret = slave_bank_setValveData(0x02, v);
//     TEST_EQUAL(ret, true);
//     v.a.state = VALVE_STATE_CLOSED;
//     v.a.timer = 40;
//     v.b.state = VALVE_STATE_CLOSED;
//     v.b.timer = 50;
//     ret = slave_bank_setValveData(0x03, v);
//     TEST_EQUAL(ret, true);
//     v.a.state = VALVE_STATE_OPEN;
//     v.a.timer = 60;
//     v.b.state = VALVE_STATE_OPEN;
//     v.b.timer = 70;
//     ret = slave_bank_setValveData(0x04, v);
//     TEST_EQUAL(ret, true);
//     ret = slave_bank_getValveData(0x01, &v);
//     TEST_EQUAL(ret, true);
//     TEST_EQUAL(v.a.state, VALVE_STATE_CLOSED);
//     TEST_EQUAL(v.b.state, VALVE_STATE_OPEN);
//     TEST_EQUAL(v.a.timer, 0);
//     TEST_EQUAL(v.b.timer, 10);
//     ret = slave_bank_getValveData(0x01, &v);
//     TEST_EQUAL(ret, true);
//     TEST_EQUAL(v.a.state, VALVE_STATE_CLOSED);
//     TEST_EQUAL(v.b.state, VALVE_STATE_OPEN);
//     TEST_EQUAL(v.a.timer, 0);
//     TEST_EQUAL(v.b.timer, 10);
//     ret = slave_bank_getValveData(0x02, &v);
//     TEST_EQUAL(ret, true);
//     TEST_EQUAL(v.a.state, VALVE_STATE_OPEN);
//     TEST_EQUAL(v.b.state, VALVE_STATE_CLOSED);
//     TEST_EQUAL(v.a.timer, 20);
//     TEST_EQUAL(v.b.timer, 30);
//     ret = slave_bank_getValveData(0x03, &v);
//     TEST_EQUAL(ret, true);
//     TEST_EQUAL(v.a.state, VALVE_STATE_CLOSED);
//     TEST_EQUAL(v.b.state, VALVE_STATE_CLOSED);
//     TEST_EQUAL(v.a.timer, 40);
//     TEST_EQUAL(v.b.timer, 50);
//     ret = slave_bank_getValveData(0x04, &v);
//     TEST_EQUAL(ret, true);
//     TEST_EQUAL(v.a.state, VALVE_STATE_OPEN);
//     TEST_EQUAL(v.b.state, VALVE_STATE_OPEN);
//     TEST_EQUAL(v.a.timer, 60);
//     TEST_EQUAL(v.b.timer, 70);
//     ret = slave_bank_getValveData(0x05, &v);
//     TEST_EQUAL(ret, false);
//     TEST_EQUAL(v.a.state, VALVE_STATE_CLOSED);
//     TEST_EQUAL(v.b.state, VALVE_STATE_CLOSED);
//     TEST_EQUAL(v.a.timer, 0);
//     TEST_EQUAL(v.b.timer, 0);
//      ret = slave_bank_getValveData(0x00, &v);
//     TEST_EQUAL(ret, false);
//     TEST_EQUAL(v.a.state, VALVE_STATE_CLOSED);
//     TEST_EQUAL(v.b.state, VALVE_STATE_CLOSED);
//     TEST_EQUAL(v.a.timer, 0);
//     TEST_EQUAL(v.b.timer, 0);
   
//     // Test timer func
//     uint8_t nof_timeouts;
//     slave_bank_init();
//     TEST_EQUAL(slave_bank_getNumberOfTimeouts(), 0);
//     slave_bank_add((struct mac_addr_s){0x11223344, 0x55667788});
//     v.a.state = VALVE_STATE_OPEN;
//     v.a.timer = 5;
//     v.b.state = VALVE_STATE_OPEN;
//     v.b.timer = 3;
//     slave_bank_setValveData(0x01, v);
//     for(int c = 3; c; c--){
//         slave_bank_timer_tick_cb();
//     }
//     slave_bank_getValveData(0x01, &v);
//     TEST_EQUAL(v.a.isTimeoutTriggered, false);
//     TEST_EQUAL(v.a.timer, 2);
//     TEST_EQUAL(v.b.isTimeoutTriggered, true);
//     TEST_EQUAL(v.b.timer, 0);
//     TEST_EQUAL(slave_bank_getNumberOfTimeouts(), 1);
//     slave_bank_setValveBStateClosed(0x01);
//     TEST_EQUAL(slave_bank_getNumberOfTimeouts(), 0);
//     slave_bank_getValveData(0x01, &v);
//     TEST_EQUAL(v.a.isTimeoutTriggered, false);
//     TEST_EQUAL(v.a.timer, 2);
//     TEST_EQUAL(v.b.isTimeoutTriggered, false);
//     TEST_EQUAL(v.b.timer, 0);
//     for(int c = 2; c; c--){
//         slave_bank_timer_tick_cb();
//     }
//     slave_bank_getValveData(0x01, &v);
//     TEST_EQUAL(v.a.isTimeoutTriggered, true);
//     TEST_EQUAL(v.a.timer, 0);
//     TEST_EQUAL(v.b.isTimeoutTriggered, false);
//     TEST_EQUAL(v.b.timer, 0);
//     TEST_EQUAL(slave_bank_getNumberOfTimeouts(), 1);
//     slave_bank_setValveAStateClosed(0x01);
//     TEST_EQUAL(slave_bank_getNumberOfTimeouts(), 0);
//     slave_bank_getValveData(0x01, &v);
//     TEST_EQUAL(v.a.isTimeoutTriggered, false);
//     TEST_EQUAL(v.a.timer, 0);
//     TEST_EQUAL(v.b.isTimeoutTriggered, false);
//     TEST_EQUAL(v.b.timer, 0);

//     slave_bank_add((struct mac_addr_s){0x12345678, 0x87654321});
//     v.a.state = VALVE_STATE_OPEN;
//     v.a.timer = 3;
//     v.b.state = VALVE_STATE_OPEN;
//     v.b.timer = 5;
//     slave_bank_setValveData(0x02, v);
//     slave_bank_add((struct mac_addr_s){0x1, 0x2});
//     v.a.state = VALVE_STATE_OPEN;
//     v.a.timer = 2;
//     v.b.state = VALVE_STATE_CLOSED;
//     v.b.timer = 4;
//     slave_bank_setValveData(0x03, v);
//     TEST_EQUAL(slave_bank_getNumberOfTimeouts(), 0);
//     for(int c = 3; c; c--){
//         slave_bank_timer_tick_cb();
//     }
//     slave_bank_getValveData(0x02, &v);
//     TEST_EQUAL(v.a.isTimeoutTriggered, true);
//     TEST_EQUAL(v.a.timer, 0);
//     TEST_EQUAL(v.b.isTimeoutTriggered, false);
//     TEST_EQUAL(v.b.timer, 2);
//     slave_bank_getValveData(0x03, &v);
//     TEST_EQUAL(v.a.isTimeoutTriggered, true);
//     TEST_EQUAL(v.a.timer, 0);
//     TEST_EQUAL(v.b.isTimeoutTriggered, false);
//     TEST_EQUAL(v.b.timer, 4);
//     TEST_EQUAL(slave_bank_getNumberOfTimeouts(), 2);
//     for(int c = 2; c; c--){
//         slave_bank_timer_tick_cb();
//     }
//     slave_bank_getValveData(0x02, &v);
//     TEST_EQUAL(v.a.isTimeoutTriggered, true);
//     TEST_EQUAL(v.a.timer, 0);
//     TEST_EQUAL(v.b.isTimeoutTriggered, true);
//     TEST_EQUAL(v.b.timer, 0);
//     slave_bank_getValveData(0x03, &v);
//     TEST_EQUAL(v.a.isTimeoutTriggered, true);
//     TEST_EQUAL(v.a.timer, 0);
//     TEST_EQUAL(v.b.isTimeoutTriggered, false);
//     TEST_EQUAL(v.b.timer, 4);
//     TEST_EQUAL(slave_bank_getNumberOfTimeouts(), 3);
//     {
//         uint8_t *timeoutids;
//         uint8_t len = slave_bank_getTimeoutedIds(&timeoutids);
//         TEST_EQUAL(len, 2);
//         uint8_t exp[] = {2,3};
//         TEST_ARR_EQ(timeoutids, exp, len);
//         free(timeoutids);
//     }
//     v.a.state = VALVE_STATE_CLOSED;
//     v.a.timer = 0;
//     v.b.state = VALVE_STATE_CLOSED;
//     v.b.timer = 0;
//     slave_bank_setValveData(2, v);
//     TEST_EQUAL(slave_bank_getNumberOfTimeouts(), 1);
//     {
//         uint8_t *timeoutids;
//         uint8_t len = slave_bank_getTimeoutedIds(&timeoutids);
//         TEST_EQUAL(len, 1);
//         uint8_t exp[] = {3};
//         TEST_ARR_EQ(timeoutids, exp, len);
//         free(timeoutids);
//     }
//     v.a.state = VALVE_STATE_CLOSED;
//     v.a.timer = 0;
//     v.b.state = VALVE_STATE_CLOSED;
//     v.b.timer = 0;
//     slave_bank_setValveData(2, v);
//     slave_bank_setValveData(3, v);
//     TEST_EQUAL(slave_bank_getNumberOfTimeouts(), 0);
//     {
//         uint8_t *timeoutids;
//         uint8_t len = slave_bank_getTimeoutedIds(&timeoutids);
//         TEST_EQUAL(len, 0);
//     }


// }

int main(){
    test_ebv_local_verify_gnss_response();
    test_ebv_local_parse_gnss_response();
    // Print summary
    printf("\nSummary: ");
    if(test_passed && !test_failed){
        printf("\033[32mAll test PASSED.\033[37m\n");
    } else if(test_passed && test_failed){
        printf("\033[32m%d PASSED\033[31m %d FAILED.\033[37m\n", test_passed, test_failed);
    }else if(!test_passed && test_failed){
        printf("\033[31mAll test FAILED\033[37m");
    } else {
        printf("No test was executed");
    }
return 0;
}