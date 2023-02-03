#include "../core/ebv_iot.c"

void ebv_iot_addGenericPayload(const char * key, unsigned int value){
        _ebv_iot_addUnsignedPayload(key, value);
    }
    void ebv_iot_addGenericPayload(const char * key, int value){
        _ebv_iot_addSignedPayload(key, value);
    }
    void ebv_iot_addGenericPayload(const char * key, float value){
        _ebv_iot_addFloatPayload(key, value);
    }
    void ebv_iot_addGenericPayload(const char * key, double value){
        _ebv_iot_addDoublePayload(key, value);
    }
    void ebv_iot_addGenericPayload(const char * key, const char * value){
        _ebv_iot_addStringPayload(key, value);
    }
    void ebv_iot_addGenericPayload(const char * key, const char value){
        _ebv_iot_addCharPayload(key, value);
    }