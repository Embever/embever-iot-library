#ifndef INC_MCP9808_H
#define INC_MCP9808_H

#include <Arduino.h>

bool mcp9808_init();
float mcp9808_get_temperature();

#endif