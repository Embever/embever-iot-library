#include "ebv_eftp.h"
#include "ebv_esp.h"
#include "extcwpack.h"
#define LOG_MODULE_NAME EBV_EFTP_LOG_NAME
#include "ebv_log.h"

#include <string.h>

static bool ebv_eftp_validate_open_response(const uint8_t *response, const uint8_t response_len);
static bool ebv_eftp_submit_esp_cmd(esp_packet_t *p, esp_response_t *r);
static void ebv_eftp_eval_error(uint8_t *esp_respose_payload);

static unsigned int _ebv_eft_last_open_file_size = 0;
static enum ebv_esp_remote_file_error_codes _ebv_eftp_last_error_code = EBV_ESP_REMOTE_FILE_ERROR_UNKNOWN;

// Open a file for read or write

bool ebv_eftp_open(const char *file_path, const char *mode){
    // Validate parameters
    uint8_t _mode;
    if(strlen(mode) > 2 || strlen(file_path) < 2){
        return false;
    }
    if(strcmp(mode, "r") == 0){
        _mode = 0;
    } else if (strcmp(mode, "w") == 0){
        _mode = 1;
    } else {
        return false;
    }
    // Build the request
    esp_packet_t pkg;
    cw_pack_context c;
    pkg.data[0] = REMOTE_FILE_CMD_OPEN;
    pkg.len = 1;
    cw_pack_context_init(&c, &pkg.data[1], sizeof(pkg.data) - 1, NULL);
    cw_pack_array_size(&c, 2);
    cw_pack_str(&c, file_path, strlen(file_path));
    cw_pack_unsigned(&c, _mode);
    pkg.len += c.current - c.start;
    // Pack into ESP and send it
    esp_response_t response;
    bool ret = ebv_eftp_submit_esp_cmd(&pkg, &response);
    if(!ret){
        DEBUG_MSG_TRACE("ESP communication failed");
        return false;
    }
    DEBUG_MSG_TRACE("EFTP file open response payload with len: %d", response.payload_len);
#if DEBUG_EN == 1
    for(int i = 0; i < response.payload_len; i++){
        if( !(i%8)){ DEBUG_MSG("\n\r"); }
        DEBUG_MSG("%X ", response.payload[i]);
    }
    DEBUG_MSG("\n\r");
#endif
    if(response.has_error_code){
        ebv_eftp_eval_error(response.payload);
        return false;
    }

    return ebv_eftp_validate_open_response(response.payload, response.payload_len);
}

bool ebv_eftp_open_with_size(char *filename, const char *mode, const int size){
    return true;
}

bool ebv_eftp_write(char *file_data, unsigned int file_data_len){
    esp_packet_t pkg;
    if(file_data_len > sizeof(pkg.data) -1 ){
        return false;
    }

    pkg.data[0] = REMOTE_FILE_CMD_WRITE;
    pkg.len = 1;
    memcpy(&(pkg.data[1]), file_data, file_data_len);
    pkg.len += file_data_len;
    esp_response_t response;
    bool ret = ebv_eftp_submit_esp_cmd(&pkg, &response);
    if(!ret){
        return false;
    }

    // check file write operation result
    if(response.has_error_code){
        ebv_eftp_eval_error(response.payload);
        return false;
    }

    return true;
}

uint8_t ebv_eftp_read(char *data, int data_len){
    return 0;
}

bool ebv_eftp_close(){
    esp_packet_t pkg;
    pkg.data[0] = REMOTE_FILE_CMD_WRITE_DONE;
    pkg.len = 1;
    return ebv_eftp_submit_esp_cmd(&pkg, NULL);
}

enum ebv_esp_remote_file_error_codes ebv_eftp_get_latest_error_code(){
    return _ebv_eftp_last_error_code;
}

static bool ebv_eftp_submit_esp_cmd(esp_packet_t *pkg, esp_response_t *r){
    if(r == NULL){
        esp_response_t response;
        r = &response;
    }
    ebv_esp_packetBuilderByArray(pkg, ESP_CMD_READ_LOCAL_FILE, (uint8_t *) pkg->data, pkg->len);
    if( !ebv_esp_submitPacket(pkg) ){
        DEBUG_MSG_TRACE("Failed to submit package");
        return false;
    }
    if( !ebv_esp_queryDelayedResponse(r) ){
        DEBUG_MSG_TRACE("Failed to read delayed response");
        return false;
    }
    // verify
    ebv_esp_resp_res_t res = ebv_esp_eval_delayed_resp(r, ESP_CMD_READ_LOCAL_FILE);
    if(res != EBV_ESP_RESP_RES_OK ){
        DEBUG_MSG_TRACE("Delayed response validation failed");
        return false;
    }

    return true;
}

static bool ebv_eftp_validate_open_response(const uint8_t *response, const uint8_t response_len){
    if(response_len < 3){
        DEBUG_MSG_TRACE("Response too short: &d", response_len);
        return false;
    }
    cw_unpack_context uc;
    cw_unpack_context_init(&uc, (void *)response, response_len, NULL);
    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_ARRAY && uc.item.as.array.size != 2){
        DEBUG_MSG_TRACE("Failed to parse response");
        return false;
    }
    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){
        DEBUG_MSG_TRACE("Failed to parse response");
        return false;
    }
    enum ebv_esp_file_result_code result = (enum ebv_esp_file_result_code) uc.item.as.u64;

    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){
        DEBUG_MSG_TRACE("Failed to parse response");
        return false;
    }
    _ebv_eft_last_open_file_size = (unsigned int) uc.item.as.u64;

    // evaluate file operation result code
    if(result != EBV_ESP_FILE_OK){
        return false;
    }

    return true;
}

static void ebv_eftp_eval_error(uint8_t *esp_respose_payload){
    uint16_t esp_error_code = (esp_respose_payload[0] << 8) | esp_respose_payload[1];
    switch (esp_error_code){
        case ESP_ERR_RESOURCE_BUSY:
            _ebv_eftp_last_error_code = EBV_ESP_REMOTE_FILE_ERROR_RESOURCE_BUSY;
            break;
        case ESP_ERR_INVALID_CMD_DATA:
            _ebv_eftp_last_error_code = EBV_ESP_REMOTE_FILE_ERROR_INVALID_REQUEST;
            break;
        case ESP_ERR_INTERNAL_ERROR:
            _ebv_eftp_last_error_code = EBV_ESP_REMOTE_FILE_ERROR_OPEN_FAILED;
            break;
        default:
            _ebv_eftp_last_error_code = EBV_ESP_REMOTE_FILE_ERROR_UNKNOWN;
        break;
    }

    return;
}