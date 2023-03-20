#ifndef INC_EBV_UTIL_H
#define INC_EBV_UTIL_H

#include "ebv_esp.h"
#include "ebv_local.h"

#define DEFAULT_NETWORK_ATTACH_TIMEOUT_SEC  120

bool ebv_util_is_network_ready();
enum ebv_general_status ebv_util_get_general_device_status();
bool ebv_util_wait_device_ready(unsigned int timeout_sec);


#endif