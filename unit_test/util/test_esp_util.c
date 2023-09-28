#include "test_esp_util.h"
#include "mock_ebv_i2c.h"

uint16_t test_esp_util_build_delayed_response(char *pkg_buffer, char trigger_cmd, const char * payload, uint16_t payload_len){
    uint16_t pkg_buffer_index = 0;
    // Delayed header
    const char delayed_header[] = { _MOCK_I2C_DELAYED_RESPONSE_HEADER };
    memcpy(&(pkg_buffer[pkg_buffer_index]), delayed_header, sizeof(delayed_header));
    pkg_buffer_index += sizeof(delayed_header);
    // Delayed header len
    const char trigger_cmd_header[] = {0x55, trigger_cmd};
    const uint8_t trigger_cmd_len_size = 2;
    const char pkg_crc[] = {_MOCK_I2C_CRC};
    const char pkg_flag[] = {_MOCK_I2C_FLAG};
    const uint16_t delayed_header_len = payload_len + sizeof(trigger_cmd_header) + trigger_cmd_len_size + sizeof(pkg_crc) + sizeof(pkg_flag);
    pkg_buffer[pkg_buffer_index] = (char) (delayed_header_len & 0xFF);
    pkg_buffer[pkg_buffer_index + 1] = (char) ((delayed_header_len >> 8) & 0xFF);
    pkg_buffer_index += 2;
    // Trigger cmd header
    memcpy(&(pkg_buffer[pkg_buffer_index]), trigger_cmd_header, sizeof(trigger_cmd_header));
    pkg_buffer_index += sizeof(trigger_cmd_header);
    // Trigger cmd header length
    const uint16_t trigger_cmd_header_len = payload_len + sizeof(pkg_crc) + sizeof(pkg_flag);
    pkg_buffer[pkg_buffer_index] = (char) (trigger_cmd_header_len & 0xFF);
    pkg_buffer[pkg_buffer_index + 1] = (char) ((trigger_cmd_header_len >> 8) & 0xFF);
    pkg_buffer_index += 2;

    // Copy payload
    memcpy(&pkg_buffer[pkg_buffer_index], payload, payload_len);
    pkg_buffer_index += payload_len;
    // Copy crc
    memcpy(&pkg_buffer[pkg_buffer_index], pkg_crc, sizeof(pkg_crc));
    pkg_buffer_index += sizeof(pkg_crc);
    // Copy flags
    memcpy(&pkg_buffer[pkg_buffer_index], pkg_flag, sizeof(pkg_flag));
    pkg_buffer_index += sizeof(pkg_flag);

    return pkg_buffer_index;
}