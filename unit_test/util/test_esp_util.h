#ifndef TEST_ESP_UTIL_H
#define TEST_ESP_UTIL_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

uint16_t test_esp_util_build_delayed_response(char pkg_buffer, char trigger_cmd, const char * payload, uint16_t payload_len);

#endif TEST_ESP_UTIL_H