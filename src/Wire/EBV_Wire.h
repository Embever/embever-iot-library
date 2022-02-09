#ifndef INC_EBV_WIRE_H
#define INC_EBV_WIRE_H

#if defined(ESP32)
#define I2C_BUFFER_LENGTH 512
#include "ESP32/Wire_ESP32.h"
#elif defined(__AVR__)
#include "AVR/Wire_AVR.h"
#endif

#endif