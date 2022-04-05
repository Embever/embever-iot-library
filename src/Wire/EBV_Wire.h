#ifndef INC_EBV_WIRE_H
#define INC_EBV_WIRE_H

#include "ebv_conf.h"

#if !defined(I2C_BUFFER_LEN)
    #define I2C_BUFFER_LEN DEFAULT_EBV_I2C_BUFFER_LEN
#endif

#if defined(ESP32)
#include "ESP32/Wire_ESP32.h"
#elif defined(__AVR__)
#include "AVR/Wire_AVR.h"
#endif

#endif