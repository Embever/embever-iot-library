#ifndef INC_EBV_CONF_H
#define INC_EBV_CONF_H

#include "ebv_esp_conf.h"

#define MIN_IOT_MSG_MAX_LEN      64
#define MED_IOT_MSG_MAX_LEN     256
#define MAX_IOT_MSG_MAX_LEN     512


#if defined(ESP32)
    #define IOT_MSG_MAX_LEN MAX_IOT_MSG_MAX_LEN
#elif defined(__AVR__)
    #define IOT_MSG_MAX_LEN MIN_IOT_MSG_MAX_LEN
#else
    #define IOT_MSG_MAX_LEN MIN_IOT_MSG_MAX_LEN
#endif

#define DEFAULT_EBV_I2C_BUFFER_LEN      (IOT_MSG_MAX_LEN + ESP_PACKET_OVERHEAD)


#endif