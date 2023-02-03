#ifndef INC_EBV_CONF_H
#define INC_EBV_CONF_H

#include "ebv_esp_conf.h"

#define MIN_IOT_MSG_MAX_LEN      64
#define MED_IOT_MSG_MAX_LEN     256
#define MAX_IOT_MSG_MAX_LEN     512

// Possible return values from ebv functions
typedef enum{
    EBV_RET_OK,                         // All OK
    EBV_RET_OK_WITH_ERROR,              // ESP com was successful but there was an error durring data exchange with cloud
    EBV_RET_NO_ACTION,                  // No action in the cloud for this device
    EBV_RET_ESP_NO_ACK,                 // Failed to receive ACK packet
    EBV_RET_ESP_NO_DEL_RESP,            // There is no delayed response
    EBV_RET_I2C_NO_RESP,                // Device not responding to command, i2c communication failure
    EBV_RET_INV_ACK,                    // Invalid ACK received
    EBV_RET_INV_DEL_RESP,               // Invalid delayed response received
    EBV_RET_INV_PAYLOAD,                // Invalid payload, mostly means that the payload of the delayed response is wrong
    EBV_RET_INV_ACTION,                 // Action parser can not validate the action
    EBV_RET_ERROR,                      // General error
    EBV_RET_TIMEOUT                     // Timeout durring waiting for device
} ebv_ret_t;

#define EBV_ESP_CONNECTIVITY_TIMEOUT    60

#if defined(ESP32)
    #define IOT_MSG_MAX_LEN MAX_IOT_MSG_MAX_LEN
#elif defined(__AVR__)
    #define IOT_MSG_MAX_LEN MIN_IOT_MSG_MAX_LEN
#elif defined(STM32F103xB)
    #define IOT_MSG_MAX_LEN MAX_IOT_MSG_MAX_LEN
#else
    #define IOT_MSG_MAX_LEN MIN_IOT_MSG_MAX_LEN
#endif

#define DEFAULT_EBV_I2C_BUFFER_LEN      (IOT_MSG_MAX_LEN + ESP_PACKET_OVERHEAD)


#endif