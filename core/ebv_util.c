#include "ebv_util.h"
#include "ebv_delay.h"

bool ebv_util_is_network_ready(){
    ebv_local_device_status_t status;
    bool ret = ebv_local_status_update(&status);
    if(ret == false){
        return false;
    }
    if( (status.modem_status.network_status == EBV_MODEM_NETWORK_STATUS_REGISTERED_HOME || 
        status.modem_status.network_status == EBV_MODEM_NETWORK_STATUS_REGISTERED_ROAMING)
                                            &&
        (status.general_status.status == EBV_GENERAL_STATUS_READY ||
        status.general_status.status == EBV_GENERAL_STATUS_GPS_ACTIVE)
        )
    {
        return true;
    }

    return false;
}

enum ebv_general_status ebv_util_get_general_device_status(){
     ebv_local_device_status_t status;
    bool ret = ebv_local_status_update(&status);
    if(ret == false){
        return EBV_GENERAL_STATUS_UNKNOWN;
    }

    return status.general_status.status;
}

bool ebv_util_wait_device_ready(unsigned int timeout_sec){
    enum ebv_general_status gen_status = ebv_util_get_general_device_status();
    while(timeout_sec && (gen_status != EBV_GENERAL_STATUS_READY && gen_status != EBV_GENERAL_STATUS_GPS_ACTIVE)){
        if(gen_status == EBV_GENERAL_STATUS_BUSY_FOTA){
            return false;
        }
        ebv_delay(2000);
        gen_status = ebv_util_get_general_device_status();
        timeout_sec--;
    }

    return timeout_sec != 0;
}