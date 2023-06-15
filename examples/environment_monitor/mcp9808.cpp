#include "mcp9808.h"

#include <Wire.h>

#define MCP9808_ADDR    0x18
#define MCP9808_MID     0x0054

enum{
    mcp9808_reg_temperature     = 5,
    mcp9808_reg_manufacturer_id = 6
};

bool mcp9808_init(){
    Wire.beginTransmission(MCP9808_ADDR);
    Wire.write(mcp9808_reg_manufacturer_id);
    Wire.endTransmission();
    Wire.requestFrom(MCP9808_ADDR, 2);
    delay(2);
    if(Wire.available() != 2){
        return false;
    }

    uint16_t mid = (uint16_t) (Wire.read()) << 8 | Wire.read();
    if(mid != MCP9808_MID){
        return false;
    }

    return true;
}

float mcp9808_get_temperature(){
    Wire.beginTransmission(MCP9808_ADDR);
    Wire.write(mcp9808_reg_temperature);
    Wire.endTransmission();
    Wire.requestFrom(MCP9808_ADDR, 2);
    delay(2);
    if(Wire.available() != 2){
        return false;
    }

    uint16_t t = ((uint16_t) Wire.read()) << 8 | Wire.read();
    float temperature = 0;

    if (t != 0xFFFF) {
      temperature = t & 0x0FFF;
      temperature /= 16.0;
      if (t & 0x1000)
        temperature -= 256;
    }

    return temperature;
}