/*
 * Copyright (c) 2017 - 2021, Embever GmbH
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "ebv_local.h"
#include "ebv_esp.h"
#include "ebv_delay.h"
#include <extcwpack.h>
#define LOG_MODULE_NAME EBV_LOCAL_LOG_NAME
#include "ebv_log.h"

static bool ebv_local_gnss_submit(esp_response_t *response);
#ifndef EBV_UNIT_TEST
static bool _ebv_local_parse_gnss_response(esp_response_t *response, ebv_gnss_data_t *pvt);
static bool _ebv_local_verify_gnss_response(uint8_t *payload, uint8_t payload_len, ebv_gnss_request_kind query_type, ebv_gnss_data_t *pvt);
static bool _ebv_esp_status_parser(uint8_t *data, uint16_t len, ebv_local_device_status_t * status);
static void _ebv_local_status_parser_modem(cw_unpack_context *uc, ebv_local_modem_status_t *status);
static void _ebv_local_status_parser_general(cw_unpack_context *uc, ebv_local_general_status_t *status);
#endif

extern esp_err_t esp_err;

uint8_t _gnss_query_data[32];
uint8_t _gnss_query_data_len;
uint8_t _gnss_nof_queries;
uint8_t *_gnss_queries;

static bool ebv_local_query_gnss_submit(ebv_gnss_data_t *pvt);

bool ebv_local_query_gnss(ebv_gnss_data_t *pvt){
    _gnss_query_data[0] = 0x91;
    _gnss_query_data[1] = 0x93;
    _gnss_query_data[2] = EBV_GNSS_REQUEST_LOCATION;
    _gnss_query_data[3] = EBV_GNSS_REQUEST_SPEED;
    _gnss_query_data[4] = EBV_GNSS_REQUEST_DATETIME;
    _gnss_queries = &(_gnss_query_data[2]);
    _gnss_nof_queries = 3;
    _gnss_query_data_len = 5;
    return ebv_local_query_gnss_submit(pvt);
}

bool ebv_local_query_gnss_cont(ebv_gnss_data_t *pvt){
    cw_pack_context c;
    cw_pack_context_init(&c, _gnss_query_data, sizeof(_gnss_query_data), NULL);
    cw_pack_array_size(&c, 2);
    cw_pack_array_size(&c, 3);
    _gnss_queries = c.current;
    cw_pack_unsigned(&c, EBV_GNSS_REQUEST_LOCATION);
    cw_pack_unsigned(&c, EBV_GNSS_REQUEST_SPEED);
    cw_pack_unsigned(&c, EBV_GNSS_REQUEST_DATETIME);
    _gnss_nof_queries = 3;
    // optional arguments
    cw_pack_map_size(&c, 1);
    cw_pack_str(&c, EBV_GNSS_REQUEST_OPTIONAL_PARAM_NAV_MODE, sizeof(EBV_GNSS_REQUEST_OPTIONAL_PARAM_NAV_MODE) - 1);
    cw_pack_unsigned(&c, EBV_GNSS_NAV_MODE_CONTINUOUS);
    _gnss_query_data_len = (uint8_t) (c.current - c.start);
    return ebv_local_query_gnss_submit(pvt);
}

bool ebv_report_pvt(){
    _gnss_query_data[0] = 0x91;
    _gnss_query_data[1] = 0x91;
    _gnss_query_data[2] = EBV_GNSS_REPORT_LOCATION;
    _gnss_queries = &(_gnss_query_data[2]);
    _gnss_nof_queries = 1;
    _gnss_query_data_len = 3;
    esp_response_t resp;
    return ebv_local_gnss_submit(&resp);
}

void ebv_local_query_gnss_custom_init(){
    memset(_gnss_query_data, 0, sizeof(_gnss_query_data));
    _gnss_query_data[0] = 0x91;
    _gnss_query_data[1] = 0x90;
    _gnss_queries = &(_gnss_query_data[2]);
    _gnss_query_data_len = 2;
    _gnss_nof_queries = 0;
}

bool ebv_local_query_gnss_custom_add(ebv_gnss_request_kind k ){
    if(_gnss_query_data_len >= sizeof(_gnss_query_data)){
        DEBUG_MSG_TRACE("No more query can fit");
        return false;
    }
    _gnss_query_data[_gnss_query_data_len] = k;
    _gnss_query_data_len++;
    _gnss_nof_queries++;
    return true;
}

bool ebv_local_query_gnss_custom_add_submit(ebv_gnss_data_t *pvt){
    _gnss_query_data[1] += _gnss_nof_queries;
    return ebv_local_query_gnss_submit(pvt);
}

bool ebv_query_gps_status(ebv_gps_status_t *status){
    _gnss_query_data[0] = 0x91;
    _gnss_query_data[1] = EBV_GNSS_REQUEST_STATUS;
    _gnss_nof_queries = 1;
    _gnss_query_data_len = 2;
    esp_response_t resp;
    bool ret = ebv_local_gnss_submit(&resp);
    if(!ret){
        return false;
    }
    // Parsing response
    cw_unpack_context uc;
    cw_unpack_context_init(&uc, resp.payload, resp.payload_len, NULL);
    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_ARRAY){
        return false;
    }

    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){
        return false;
    }
    status->state = (uint8_t) uc.item.as.u64;

    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_BOOLEAN){
        return false;
    }
    status->is_last_fix_success = uc.item.as.boolean;

    return true;
}

bool ebv_local_set_op_mode(ebv_local_pwr_op_mode op_mode){
    esp_packet_t pkg;
    if(op_mode == EBV_OP_MODE_PWR_DOWN){
        pkg.data[0] = 0x91;
        pkg.data[1] = 0xC3;
        ebv_esp_packetBuilderByArray(&pkg, ESP_CMD_PWR_MODE, pkg.data, 2);
        if( !ebv_esp_submitPacket(&pkg) ){
            DEBUG_MSG_TRACE("Failed to submit package");
            return false;
        }
    } else {
        // The device is sleeping, try to send a long LOW pulse over I2C to wake it up
        ebv_esp_wakeup_device();
        ebv_delay(10 * 1000);
        return waitForDevice();
    }

    return true;
}

static bool ebv_local_gnss_submit(esp_response_t *response){
    esp_packet_t pkg;
    ebv_esp_packetBuilderByArray(&pkg, ESP_CMD_UPDATE_GNSS_LOCATION, (uint8_t *) &_gnss_query_data, _gnss_query_data_len);
    if( !ebv_esp_submitPacket(&pkg) ){
        DEBUG_MSG_TRACE("Failed to submit package");
        return false;
    }
    if( !ebv_esp_queryDelayedResponse(response) ){
        DEBUG_MSG_TRACE("Failed to read delayed response");
        return false;
    }
    // verify
    ebv_esp_resp_res_t res = ebv_esp_eval_delayed_resp(response, ESP_CMD_UPDATE_GNSS_LOCATION);
    if(res != EBV_ESP_RESP_RES_OK ){
        DEBUG_MSG_TRACE("Delayed response validation failed");
        return false;
    }
    DEBUG_MSG_TRACE("GNSS response payload with len: %d", response->payload_len);
#if DEBUG_EN == 1
    for(int i = 0; i < response->payload_len; i++){
        if( !(i%8)){ DEBUG_MSG("\n\r"); }
        DEBUG_MSG("%X ", response->payload[i]);
    }
    DEBUG_MSG("\n\r");
#endif
    return true;
}

static bool ebv_local_query_gnss_submit(ebv_gnss_data_t *pvt){
    esp_response_t response;
    if( ebv_local_gnss_submit(&response) == false){
        return false;
    }
    // Parse response
    return _ebv_local_parse_gnss_response(&response, pvt);
}

ebv_unit_test_static bool _ebv_local_parse_gnss_response(esp_response_t *response, ebv_gnss_data_t *pvt){
    cw_unpack_context uc;
    cw_unpack_context_init(&uc, response->payload, response->payload_len, NULL);
    cw_unpack_next(&uc);
    if(uc.item.as.array.size != _gnss_nof_queries ){
        DEBUG_MSG_TRACE("Failed to verify response, insufficent queries received");
        return false;
    }
    for(uint8_t i = 0; i < _gnss_nof_queries; i++){
        bool ret = _ebv_local_verify_gnss_response( uc.current,
                                                    response->payload_len - (uc.current - uc.start),
                                                    (ebv_gnss_request_kind)_gnss_queries[i],
                                                    pvt
        );
        if(!ret){
            return false;
        }
        cw_skip_items(&uc, 1);
    }
    return true;
}

ebv_unit_test_static bool _ebv_local_verify_gnss_response(uint8_t *payload, uint8_t payload_len, ebv_gnss_request_kind query_type, ebv_gnss_data_t *pvt){
    cw_unpack_context uc;
    cw_unpack_context_init(&uc, payload, payload_len, NULL);
    if(query_type == EBV_GNSS_REQUEST_LOCATION){
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_ARRAY || uc.item.as.array.size != 3){
            DEBUG_MSG_TRACE("Response validation failed, root array");
            return false;
        }
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_ARRAY || uc.item.as.array.size != 2){
            DEBUG_MSG_TRACE("Response validation failed, sub array");
            return false;
        }
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_DOUBLE){
            DEBUG_MSG_TRACE("Response validation failed, sub array item 1");
            return false;
        }
        pvt->lat = uc.item.as.long_real;
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_DOUBLE){
            DEBUG_MSG_TRACE("Response validation failed, sub array item 2");
            return false;
        }
        pvt->lon = uc.item.as.long_real;
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_FLOAT){
            DEBUG_MSG_TRACE("Response validation failed, item 2");
            return false;
        }
        pvt->altitude = uc.item.as.real;
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_FLOAT){
            DEBUG_MSG_TRACE("Response validation failed, item 3");
            return false;
        }
        pvt->accuracy = uc.item.as.real;
        if(pvt->lat == 0 || pvt->lon == 0 || pvt->accuracy == 0){
            pvt->has_fix = false;
        } else {
            pvt->has_fix = true;
        }
        return true;
    } else if(query_type == EBV_GNSS_REQUEST_SPEED){
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_ARRAY || uc.item.as.array.size != 2){
            DEBUG_MSG_TRACE("Response validation failed, root array");
            return false;
        }
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_FLOAT){
            return false;
        }
        pvt->speed = uc.item.as.real;
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_FLOAT){
            return false;
        }
        pvt->heading = uc.item.as.real;
        return true;
     } else if(query_type == EBV_GNSS_REQUEST_DATETIME){
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_ARRAY || uc.item.as.array.size != 2){
            return false;
        }
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_ARRAY || uc.item.as.array.size != 3){
            return false;
        }
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){
            return false;
        }
        pvt->datetime.year = uc.item.as.u64;
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){
            return false;
        }
        pvt->datetime.month = uc.item.as.u64;
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){
            return false;
        }
        pvt->datetime.day = uc.item.as.u64;
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_ARRAY || uc.item.as.array.size != 3){
            return false;
        }
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){
            return false;
        }
        pvt->datetime.hour = uc.item.as.u64;
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){
            return false;
        }
        pvt->datetime.minute = uc.item.as.u64;
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){
            return false;
        }
        pvt->datetime.seconds = uc.item.as.u64;
        return true;
    } else if(query_type == EBV_GNSS_REQUEST_EXTENDED){
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_ARRAY || uc.item.as.array.size != 3){
            DEBUG_MSG_TRACE("Response validation failed, root array");
            return false;
        }
        // Validate the location part
        bool ret = _ebv_local_verify_gnss_response(uc.current, payload_len - (uc.current - uc.start) , EBV_GNSS_REQUEST_LOCATION, pvt);
        if(!ret){
            return false;
        }
        cw_skip_items(&uc, 1);
        ret = _ebv_local_verify_gnss_response(uc.current, payload_len - (uc.current - uc.start) , EBV_GNSS_REQUEST_SPEED, pvt);
        if(!ret){
            return false;
        }
        cw_skip_items(&uc, 1);
        ret = _ebv_local_verify_gnss_response(uc.current, payload_len - (uc.current - uc.start) , EBV_GNSS_REQUEST_DATETIME, pvt);
        if(!ret){
            return false;
        }
        return true;
    }else if(query_type == EBV_GNSS_REQUEST_STATUS){
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_ARRAY || uc.item.as.array.size != 2){
            DEBUG_MSG_TRACE("Response validation failed, root array");
            return false;
        }
        // GPS status
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){
            return false;
        }
        pvt->status.state = (uint8_t) uc.item.as.u64;
        // State of the fix
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_BOOLEAN){
            return false;
        }
        pvt->status.state = (bool) uc.item.as.boolean;
        
        return true;
    } else {
        DEBUG_MSG_TRACE("Unsupported request kind : %d", query_type);
        return false;
    }

    return true;
}

bool ebv_local_set_rf_mode(enum ebv_modem_rf_mode rf_mode){
    if(rf_mode >= EBV_MODEM_RF_MODE_COUNT){
        return false;
    }
    // Keep it simple for now since there is only 1 configuration option available yet
    uint8_t cfg_data[] = {0x91, 0x92, 0xA5, 0x6D, 0x6F, 0x64, 0x65, 0x6D, 0x81, 0xA7, 0x72, 0x66, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x00};
    cfg_data[17] = rf_mode;
    esp_packet_t pkg;
    ebv_esp_packetBuilderByArray(&pkg, ESP_CMD_CONFIG, cfg_data, sizeof(cfg_data));

    // Send packet
    if( !ebv_esp_submitPacket(&pkg) ){
        DEBUG_MSG_TRACE("Failed to submit package");
        return false;
    }

    // Read response
    esp_response_t response;
    if( !ebv_esp_queryDelayedResponse(&response) ){
        DEBUG_MSG_TRACE("Failed to read delayed response");
        return false;
    }

    // Verify
    ebv_esp_resp_res_t res = ebv_esp_eval_delayed_resp(&response, ESP_CMD_CONFIG);
    if(res != EBV_ESP_RESP_RES_OK ){
        DEBUG_MSG_TRACE("Delayed response validation failed");
        return false;
    }
#if DEBUG_EN == 1
    if(response.has_error_code){
        esp_err_t err = ebv_esp_get_delayed_resp_err_code(response.payload);
        DEBUG_MSG_TRACE("Delayed response error code: %d", err);
        return false;
    }
#endif

    return true;
}

bool ebv_local_status_update(ebv_local_device_status_t * status){
    status->modem_status.rf_mode = EBV_MODEM_RF_MODE_INVALID;
    status->modem_status.lte_mode = EBV_MODEM_LTE_MODE_INVALID;
    status->modem_status.network_status = EBV_MODEM_NETWORK_STATUS_INVALID;
    status->modem_status.reset_loop_restriction_active = false;
    status->general_status.status = EBV_GENERAL_STATUS_INVALID;
    // Keep it simple for now since there is no other way to submit status read
    uint8_t status_read_data[] = {0x92, 0x91, 0xA5, 0x6D, 0x6F, 0x64, 0x65, 0x6D, 0x91, 0xA7, 0x67, 0x65, 0x6E, 0x65, 0x72, 0x61, 0x6C};
    esp_packet_t pkg;
    ebv_esp_packetBuilderByArray(&pkg, ESP_CMD_STATUS, status_read_data, sizeof(status_read_data));

    // Send packet
    if( !ebv_esp_submitPacket(&pkg) ){
        DEBUG_MSG_TRACE("Failed to submit package");
        return false;
    }

    // Read response
    esp_response_t response;
    if( !ebv_esp_queryDelayedResponse(&response) ){
        DEBUG_MSG_TRACE("Failed to read delayed response");
        return false;
    }

    // Verify
    ebv_esp_resp_res_t res = ebv_esp_eval_delayed_resp(&response, ESP_CMD_STATUS);
    if(res != EBV_ESP_RESP_RES_OK ){
        DEBUG_MSG_TRACE("Delayed response validation failed");
        return false;
    }
#if DEBUG_EN == 1
    if(response.has_error_code){
        esp_err_t err = ebv_esp_get_delayed_resp_err_code(response.payload);
        DEBUG_MSG_TRACE("Delayed response error code: %d", err);
        return false;
    }
#endif
    if(response.has_error_code){
        esp_err = ebv_esp_get_delayed_resp_err_code(response.payload);
        DEBUG_MSG_TRACE("Error code received : %d", esp_err);
        return false;
    }

    bool ret = _ebv_esp_status_parser(response.payload, response.payload_len, status);
    if(ret == false){
        DEBUG_MSG_TRACE("Status response parsing failed");
        return false;
    }

    return true;
}

// main status response parser
ebv_unit_test_static bool _ebv_esp_status_parser(uint8_t *data, uint16_t len, ebv_local_device_status_t * status){
    cw_unpack_context uc;
    cw_unpack_context_init(&uc, data, len, NULL);
    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_ARRAY){
        return false;
    }
    // looping in the status groups
    const uint8_t nof_group_items =  (uint8_t) uc.item.as.array.size;
    for(uint8_t i = 0; i < nof_group_items; i++){
        const uint8_t * status_group = uc.current;
        cw_unpack_next(&uc);
        if(uc.item.type == CWP_ITEM_ARRAY && uc.item.as.array.size == 2){
            cw_unpack_next(&uc);
            if( uc.item.type == CWP_ITEM_STR ){
                if(strncmp((const char *) uc.item.as.str.start, EBV_STATUS_GROUP_MODEM_KEY, sizeof(EBV_STATUS_GROUP_MODEM_KEY) - 1) == 0){
                    cw_unpack_next(&uc);
                    _ebv_local_status_parser_modem(&uc, &(status->modem_status));
                } else if(strncmp((const char *) uc.item.as.str.start, EBV_STATUS_GROUP_GENERAL_KEY, sizeof(EBV_STATUS_GROUP_GENERAL_KEY) - 1) == 0){
                    cw_unpack_next(&uc);
                    _ebv_local_status_parser_general(&uc, &(status->general_status));
                } else {
                    // Unknown group key in response
                }
            } else {
                // Group item key not found, maybe indicating an error?
            }
        } else {
            // Invalid status response item, maybe indicating an error?
        }

        uc.current = (uint8_t * ) status_group;
        cw_skip_items(&uc, 1);
    }

    return true;
}

ebv_unit_test_static void _ebv_local_status_parser_modem(cw_unpack_context *uc, ebv_local_modem_status_t *status){
    // iterate the response object
    if(uc->item.type == CWP_ITEM_MAP){
        const uint8_t nof_status_records = uc->item.as.map.size;
        for(uint8_t i = 0; i < nof_status_records; i++){
            const uint8_t * status_record = uc->current;
            cw_unpack_next(uc);
            if(uc->item.type == CWP_ITEM_STR){
                if(strncmp((const char *) uc->item.as.str.start, EBV_STATUS_MODEM_RF_MODE_KEY, sizeof(EBV_STATUS_MODEM_RF_MODE_KEY) - 1) == 0){
                    cw_unpack_next(uc);
                    if(uc->item.type == CWP_ITEM_POSITIVE_INTEGER && uc->item.as.u64 <= EBV_MODEM_RF_MODE_COUNT){
                        status->rf_mode = (enum ebv_modem_rf_mode) uc->item.as.u64;
                    }
                } else if( strncmp((const char *) uc->item.as.str.start, EBV_STATUS_MODEM_LTE_STATUS_KEY, sizeof(EBV_STATUS_MODEM_LTE_STATUS_KEY) - 1) == 0 ){
                    cw_unpack_next(uc);
                    if(uc->item.type == CWP_ITEM_POSITIVE_INTEGER && uc->item.as.u64 <= EBV_MODEM_LTE_MODE_COUNT){
                        status->lte_mode = (enum ebv_modem_lte_mode) uc->item.as.u64;
                    }
                } else if (strncmp((const char *) uc->item.as.str.start, EBV_STATUS_MODEM_NET_STATUS_KEY, sizeof(EBV_STATUS_MODEM_NET_STATUS_KEY) - 1) == 0){
                    cw_unpack_next(uc);
                    if(uc->item.type == CWP_ITEM_POSITIVE_INTEGER && uc->item.as.u64 <= EBV_MODEM_NETWORK_STATUS_COUNT){
                        status->network_status = (enum ebv_modem_network_status) uc->item.as.u64;
                    }
                } else if (strncmp((const char *) uc->item.as.str.start, EBV_STATUS_MODEM_RLR_STATUS_KEY, sizeof(EBV_STATUS_MODEM_RLR_STATUS_KEY) - 1) == 0){
                    if(uc->item.type == CWP_ITEM_BOOLEAN){
                        status->reset_loop_restriction_active = uc->item.as.boolean;
                    }
                }
            }
            uc->current = (uint8_t *) status_record;
            cw_skip_items(uc, 2);
        }
    }
}

ebv_unit_test_static void _ebv_local_status_parser_general(cw_unpack_context *uc, ebv_local_general_status_t *status){
    
    if(uc->item.type == CWP_ITEM_MAP){
        const uint8_t nof_status_records = uc->item.as.map.size;
        for(uint8_t i = 0; i < nof_status_records; i++){
            const uint8_t * status_record = uc->current;
            cw_unpack_next(uc);
            if(uc->item.type == CWP_ITEM_STR){
                if(strncmp((const char *) uc->item.as.str.start, EBV_STATUS_GENERAL_STATUS_KEY, sizeof(EBV_STATUS_GENERAL_STATUS_KEY) - 1) == 0){
                    cw_unpack_next(uc);
                    if(uc->item.type == CWP_ITEM_POSITIVE_INTEGER && uc->item.as.u64 <= EBV_GENERAL_STATUS_COUNT){
                        status->status = (enum ebv_general_status) uc->item.as.u64;
                    }
                }
            }
            uc->current = (uint8_t *) status_record;
            cw_skip_items(uc, 2);
        }
    }
}

#if EBV_STRINGIFY_EN == 1

void ebv_local_status_modem_str(ebv_local_modem_status_t *status,
                                char *lte_mode_str,
                                char *network_status_str,
                                char *rf_mode_str)
{

    switch (status->lte_mode){
        case EBV_MODEM_LTE_MODE_NB_IOT:
            memcpy(lte_mode_str, "LTE_MODE_NB_IOT", sizeof("LTE_MODE_NB_IOT"));
            break;
        case EBV_MODEM_LTE_MODE_LTEM:
            memcpy(lte_mode_str, "LTE_MODE_LTEM", sizeof("LTE_MODE_LTEM"));
            break;
        case EBV_MODEM_LTE_MODE_NONE:
            memcpy(lte_mode_str, "LTE_MODE_NONE", sizeof("LTE_MODE_NONE"));
            break;
        case EBV_MODEM_LTE_MODE_UNKNOWN:
            memcpy(lte_mode_str, "LTE_MODE_UNKNOWN", sizeof("LTE_MODE_UNKNOWN"));
            break;
        case EBV_MODEM_LTE_MODE_INVALID:
            memcpy(lte_mode_str, "LTE_MODE_INVALID", sizeof("LTE_MODE_INVALID"));
            break;
        default:
            memcpy(lte_mode_str, "error", sizeof("error"));
    }

    switch (status->network_status){
        case EBV_MODEM_NETWORK_STATUS_INITIALISED:
            memcpy(network_status_str, "NETWORK_STATUS_INITIALISED", sizeof("NETWORK_STATUS_INITIALISED"));
            break;
        case EBV_MODEM_NETWORK_STATUS_SEARCHING:
            memcpy(network_status_str, "NETWORK_STATUS_SEARCHING", sizeof("NETWORK_STATUS_SEARCHING"));
            break;
        case EBV_MODEM_NETWORK_STATUS_REGISTERED_HOME:
            memcpy(network_status_str, "NETWORK_STATUS_REGISTERED_HOME", sizeof("NETWORK_STATUS_REGISTERED_HOME"));
            break;
        case EBV_MODEM_NETWORK_STATUS_REGISTERED_ROAMING:
            memcpy(network_status_str, "NETWORK_STATUS_REGISTERED_ROAMING", sizeof("NETWORK_STATUS_REGISTERED_ROAMING"));
            break;
        case EBV_MODEM_NETWORK_STATUS_REGISTRATION_FAILED:
            memcpy(network_status_str, "STATUS_REGISTRATION_FAILED", sizeof("STATUS_REGISTRATION_FAILED"));
            break;
        case EBV_MODEM_NETWORK_STATUS_UNKNOWN:
            memcpy(network_status_str, "NETWORK_STATUS_UNKNOWN", sizeof("NETWORK_STATUS_UNKNOWN"));
            break;
        case EBV_MODEM_NETWORK_STATUS_INVALID:
            memcpy(network_status_str, "NETWORK_STATUS_INVALID", sizeof("NETWORK_STATUS_INVALID"));
            break;
        default:
            memcpy(network_status_str, "error", sizeof("error"));
    }

    switch (status->rf_mode){
        case EBV_MODEM_RF_MODE_NBIOT:
            memcpy(rf_mode_str, "RF_MODE_NBIOT", sizeof("RF_MODE_NBIOT"));
            break;
        case EBV_MODEM_RF_MODE_LTEM:
            memcpy(rf_mode_str, "RF_MODE_LTEM", sizeof("RF_MODE_LTEM"));
            break;
        case EBV_MODEM_RF_MODE_GPS_ONLY:
            memcpy(rf_mode_str, "RF_MODE_GPS_ONLY", sizeof("RF_MODE_GPS_ONLY"));
            break;
        case EBV_MODEM_RF_MODE_OFFLINE:
            memcpy(rf_mode_str, "RF_MODE_OFFLINE", sizeof("RF_MODE_OFFLINE"));
            break;
        case EBV_MODEM_RF_MODE_RADIO_TEST:
            memcpy(rf_mode_str, "RF_MODE_RADIO_TEST", sizeof("RF_MODE_RADIO_TEST"));
            break;
        case EBV_MODEM_RF_MODE_INVALID:
            memcpy(rf_mode_str, "RF_MODE_INVALID", sizeof("RF_MODE_INVALID"));
            break;
        default:
            memcpy(rf_mode_str, "error", sizeof("error"));
            break;
    }

    return;
}

void ebv_local_status_general_str(ebv_local_general_status_t *status, char *status_str){
    switch (status->status){
        case EBV_GENERAL_STATUS_CONNECTING:{
            memcpy(status_str, "EBV_GENERAL_STATUS_CONNECTING", sizeof("EBV_GENERAL_STATUS_CONNECTING"));
            break;
        }
        case EBV_GENERAL_STATUS_READY:{
            memcpy(status_str, "EBV_GENERAL_STATUS_READY", sizeof("EBV_GENERAL_STATUS_READY"));
            break;
        }
        case EBV_GENERAL_STATUS_BUSY_FOTA:{
            memcpy(status_str, "EBV_GENERAL_STATUS_BUSY_FOTA", sizeof("EBV_GENERAL_STATUS_BUSY_FOTA"));
            break;
        }
        case EBV_GENERAL_STATUS_GPS_ACTIVE:{
            memcpy(status_str, "EBV_GENERAL_STATUS_GPS_ACTIVE", sizeof("EBV_GENERAL_STATUS_GPS_ACTIVE"));
            break;
        }
        case EBV_GENERAL_STATUS_UNKNOWN:{
            memcpy(status_str, "EBV_GENERAL_STATUS_UNKNOWN", sizeof("EBV_GENERAL_STATUS_UNKNOWN"));
            break;
        }
        case EBV_GENERAL_STATUS_COUNT:{
            memcpy(status_str, "EBV_GENERAL_STATUS_COUNT", sizeof("EBV_GENERAL_STATUS_COUNT"));
            break;
        }
        case EBV_GENERAL_STATUS_INVALID:{
            memcpy(status_str, "EBV_GENERAL_STATUS_INVALID", sizeof("EBV_GENERAL_STATUS_INVALID"));
            break;
        }
        default:
            memcpy(status_str, "error", sizeof("error"));
    }

    return;
}

#endif