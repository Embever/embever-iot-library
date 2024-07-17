#include "../core/ebv_iot.c"

bool ebv_iot_addGenericPayload(const char * key, unsigned int value){
    return _ebv_iot_addUnsignedPayload(key, value);
}
bool ebv_iot_addGenericPayload(const char * key, int value){
    return _ebv_iot_addSignedPayload(key, value);
}
bool ebv_iot_addGenericPayload(const char * key, float value){
    return _ebv_iot_addFloatPayload(key, value);
}
bool ebv_iot_addGenericPayload(const char * key, double value){
    return _ebv_iot_addDoublePayload(key, value);
}
bool ebv_iot_addGenericPayload(const char * key, const char * value){
    return _ebv_iot_addStringPayload(key, value);
}
bool ebv_iot_addGenericPayload(const char * key, const char value){
    return _ebv_iot_addCharPayload(key, value);
}