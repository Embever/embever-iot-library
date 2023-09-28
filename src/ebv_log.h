#ifndef INC_EBV_LOG_H
#define INC_EBV_LOG_H

#include "ebv_log_conf.h"
#include "print_serial.h"

#if LOG_MODULE_NAME == EBV_ESP_LOG_NAME
    #if EBV_ESP_LOG_EN == 1
        #define MODULE_LOG_EN 1
    #else
        #define MODULE_LOG_EN 0
    #endif
#elif LOG_MODULE_NAME == EBV_IOT_LOG_NAME
    #if EBV_IOT_LOG_EN == 1
        #define MODULE_LOG_EN 1
    #else
        #define MODULE_LOG_EN 0
    #endif
#elif LOG_MODULE_NAME == EBV_LOCAL_LOG_NAME
    #if EBV_LOCAL_LOG_EN == 1
        #define MODULE_LOG_EN 1
    #else
        #define MODULE_LOG_EN 0
    #endif
#elif LOG_MODULE_NAME == EBV_EFTP_LOG_NAME
    #if EBV_EFTP_LOG_EN == 1
        #define MODULE_LOG_EN 1
    #else
        #define MODULE_LOG_EN 0
    #endif
#endif

#if MODULE_LOG_EN == 1
    #define DEBUG_EN 1
    #define DEBUG_MSG_TRACE(...) p("%s:%d in %s() --> ", __FILE__, __LINE__, __FUNCTION__); p(__VA_ARGS__); p("\n\r")
    #define DEBUG_MSG(...) p(__VA_ARGS__)
#else
    #define DEBUG_EN 0
    #define DEBUG_MSG_TRACE(...)
    #define DEBUG_MSG(...)
#endif

#endif