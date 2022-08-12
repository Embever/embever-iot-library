#ifndef Arduino_h
#define Arduino_h

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define LOW     0
#define HIGH    1

#define UNIT_TEST_SETUP_EBV_IOT_CB              UNIT_TEST_SETUP_ARDUINO_WIRE_CB UNIT_TEST_SETUP_ARDUINO_GPIO_CB UNIT_TEST_SETUP_DELAY_CB
#define UNIT_TEST_REGISTER_ARDUINO_CB           UNIT_TEST_I2C_REGISTER_ARDUINO_WIRE; UNIT_TEST_ESP_REGISTER_ARDUINO_GPIO_CB; UNIT_TEST_DELAY_REGISTER_ARDUINO;

#define UNIT_TEST_SETUP_ARDUINO_WIRE_CB                             \
    void wire_begin(uint8_t address){                               \
        return;                                                     \
    }                                                               \
    void wire_end(){                                                \
        return;                                                     \
    }                                                               \
    int wire_available(){                                           \
        return 0;                                                   \
    }                                                               \
    size_t wire_write(uint8_t data){                                \
        return 0;                                                   \
    }                                                               \
    size_t wire_requestFrom(uint8_t address, uint8_t nof_bytes){    \
       return 0;                                                    \
    }                                                               \
    int wire_read(){                                                \
        return 0;                                                   \
    }

#define UNIT_TEST_SETUP_ARDUINO_GPIO_CB                             \
    bool gpio_readReady(){                                          \
        return 0;                                                   \
    }                                                               \
    bool gpio_readIrq(){                                            \
        return 0;                                                   \
    }

#define UNIT_TEST_SETUP_DELAY_CB                                    \
    void __delay(unsigned long ms){                                 \
        return;                                                     \
    }                                                               \

#define UNIT_TEST_I2C_REGISTER_ARDUINO_WIRE                         \
    struct ebv_i2c_cb cb;                                           \
    cb.beginXfer = wire_begin;                                      \
    cb.stopXfer = wire_end;                                         \
    cb.dataAvailable = wire_available;                              \
    cb.dataWrite = wire_write;                                      \
    cb.dataReqest = wire_requestFrom;                               \
    cb.dataRead = wire_read;                                        \
    ebv_i2c_registerI2c(&cb);                                       \

#define UNIT_TEST_ESP_REGISTER_ARDUINO_GPIO_CB                      \
    struct ebv_esp_gpio_cb gpio_cb;                                 \
    gpio_cb.readReady = gpio_readReady;                             \
    gpio_cb.readIRQ = gpio_readIrq;                                 \
    ebv_esp_gpio_registerGpio(&gpio_cb);

#define UNIT_TEST_DELAY_REGISTER_ARDUINO  ebv_delay_register(__delay);

#endif