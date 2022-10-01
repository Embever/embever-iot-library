#include "ebv_esp_gpio.h"

bool ebv_esp_gpio_readIRQ(){
    // Response is available, GPIO level low
    return false;
}

bool ebv_esp_gpio_readReady(){
    return true;
}
