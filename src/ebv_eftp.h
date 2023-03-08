#ifndef INC_EBV_EFTP_H
#define INC_EBV_EFTP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum ebv_esp_remote_file_cmd{
    REMOTE_FILE_CMD_OPEN = 0,
    REMOTE_FILE_CMD_READ,
    REMOTE_FILE_CMD_WRITE,
    REMOTE_FILE_CMD_WRITE_DONE,
    REMOTE_FILE_CMD_ITEMS,
    REMOTE_FILE_CMD_INVALID
};

enum ebv_esp_file_result_code{
    EBV_ESP_FILE_OK = 0,
    EBV_ESP_FILE_REQUEST_PARSE_ERROR
};

enum ebv_esp_remote_file_error_codes{
    EBV_ESP_REMOTE_FILE_ERROR_UNKNOWN = 0,
    EBV_ESP_REMOTE_FILE_ERROR_RESOURCE_BUSY,
    EBV_ESP_REMOTE_FILE_ERROR_INVALID_REQUEST,
    EBV_ESP_REMOTE_FILE_ERROR_OPEN_FAILED
};

bool ebv_eftp_open(const char *file_path, const char *mode);
bool ebv_eftp_open_with_size(char *filename, const char *mode, const int size);
bool ebv_eftp_write(char *file_data, unsigned int file_data_len);
int ebv_eftp_read(char *data, int data_len);
bool ebv_eftp_close();
enum ebv_esp_remote_file_error_codes ebv_eftp_get_latest_error_code();

#endif