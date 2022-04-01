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

#ifndef EBV_UNIT_TEST
static bool _ebv_local_parse_gnss_response(esp_response_t *response, ebv_gnss_data_t *pvt);
static bool _ebv_local_verify_gnss_response(uint8_t *payload, uint8_t payload_len, ebv_gnss_request_kind query_type, ebv_gnss_data_t *pvt);
#endif

uint8_t _gnss_query_type[EBV_GNSS_REQUEST_LEN + 1];
uint8_t _gnss_query_type_len;

static bool ebv_local_query_gnss_submit(ebv_gnss_data_t *pvt);

bool ebv_local_query_gnss(ebv_gnss_data_t *pvt){
    _gnss_query_type[0] = 0x93;
    _gnss_query_type[1] = EBV_GNSS_REQUEST_LOCATION;
    _gnss_query_type[2] = EBV_GNSS_REQUEST_SPEED;
    _gnss_query_type[3] = EBV_GNSS_REQUEST_DATETIME;
    _gnss_query_type_len = 4;
    return ebv_local_query_gnss_submit(pvt);
}

void ebv_local_query_gnss_custom_init(){
    memset(_gnss_query_type, 0, sizeof(_gnss_query_type));
    _gnss_query_type[0] = 0x90;
    _gnss_query_type_len = 1;
}

bool ebv_local_query_gnss_custom_add(ebv_gnss_request_kind k ){
    if(_gnss_query_type_len >= sizeof(_gnss_query_type)){
        DEBUG_MSG_TRACE("No more query can fit");
        return false;
    }
    _gnss_query_type[_gnss_query_type_len] = k;
    _gnss_query_type_len++;
    return true;
}

bool ebv_local_query_gnss_custom_add_submit(ebv_gnss_data_t *pvt){
    _gnss_query_type[0] += _gnss_query_type_len - 1;
    return ebv_local_query_gnss_submit(pvt);
}

static bool ebv_local_query_gnss_submit(ebv_gnss_data_t *pvt){
    esp_packet_t pkg;
     
    ebv_esp_packetBuilderByArray(&pkg, ESP_CMD_UPDATE_GNSS_LOCATION, (uint8_t *) &_gnss_query_type, _gnss_query_type_len);
    if( !ebv_esp_submitPacket(&pkg) ){
        DEBUG_MSG_TRACE("Failed to submit package");
        return false;
    }
    esp_response_t response;
    if( !ebv_esp_queryDelayedResponse(&response) ){
        DEBUG_MSG_TRACE("Failed to read delayed response");
        return false;
    }
    // verify
    ebv_esp_resp_res_t res = ebv_esp_eval_delayed_resp(&response, ESP_CMD_UPDATE_GNSS_LOCATION);
    if(res != EBV_ESP_RESP_RES_OK ){
        DEBUG_MSG_TRACE("Delayed response validation failed");
        return false;
    }
    DEBUG_MSG_TRACE("Parsing response, len: %d", response.payload_len);
#if DEBUG_EN == 1
    for(int i = 0; i < response.payload_len; i++){
        if( !(i%8)){ DEBUG_MSG("\n\r"); }
        DEBUG_MSG("%X ", response.payload[i]);
    }
    DEBUG_MSG("\n\r");
#endif
    // Parse response
    return _ebv_local_parse_gnss_response(&response, pvt);
}

ebv_unit_test_static bool _ebv_local_parse_gnss_response(esp_response_t *response, ebv_gnss_data_t *pvt){
    cw_unpack_context uc;
    cw_unpack_context_init(&uc, response->payload, response->payload_len, NULL);
    cw_unpack_next(&uc);
    if(uc.item.as.array.size != (_gnss_query_type_len - 1) ){
        return false;
    }
    for(uint8_t i = 1; i < _gnss_query_type_len; i++){
        bool ret = _ebv_local_verify_gnss_response( uc.current,
                                                    response->payload_len - (uc.current - uc.start),
                                                    (ebv_gnss_request_kind)_gnss_query_type[i],
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
    }else {
        DEBUG_MSG_TRACE("Unsupported request kind");
        return false;
    }

    return true;
}