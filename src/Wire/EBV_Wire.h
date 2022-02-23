#ifndef INC_EBV_WIRE_H
#define INC_EBV_WIRE_H

#define DEFAULT_EBV_I2C_BUFFER_LEN      512
#define DEFAULT_EBV_I2C_BUFFER_LEN_AVR  64

#if defined(ESP32) && !defined(I2C_BUFFER_LEN)
#define I2C_BUFFER_LEN DEFAULT_EBV_I2C_BUFFER_LEN
#endif

#if defined(__AVR__) && !defined(I2C_BUFFER_LEN)
#define I2C_BUFFER_LEN DEFAULT_EBV_I2C_BUFFER_LEN_AVR
#endif

#if defined(ESP32)
#include "ESP32/Wire_ESP32.h"
#elif defined(__AVR__)
#include "AVR/Wire_AVR.h"
#endif

#endif