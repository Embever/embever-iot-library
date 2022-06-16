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

#include "ebv_iot.h"
#include "ebv_esp_gpio.h"
#include "print_serial.h"
#include "ebv_delay.h"
#include "extcwpack.h"
#include "ebv_conf.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "ebv_log_conf.h"
#define LOG_MODULE_NAME EBV_IOT_LOG_NAME
#include "ebv_log.h"

#define UNUSED(...)                     (void)(__VA_ARGS__)
#define FAIL_IF_NOT(test, err_code)     do {if( !(test) ){ esp_com_err = (err_code); return false; }}while(0);

#define str(x)                          #x
#define xstr(x)                         str(x)

#define EBV_MPACK_DEFAULT_SIZE          32
#define EBV_STRLEN_MAXLEN               64
#define EBV_MPACK_DATATYPE_HEADER        1
#define EBV_MPACK_FLOAT_LEN              4
#define EBV_MPACK_DOUBLE_LEN             8

#define MODULE_DEBUG 0

ebv_esp_com_error_t esp_com_err;

typedef struct{
    uint8_t buff[IOT_MSG_MAX_LEN];          // This is the mpack buffer
    uint16_t size;                          // The current size of the buffer
    uint8_t elements;                       // Count of the elements in the mpacked content
    bool isBufferReady;                     // Flag for indicating the budder status
    cw_pack_context c;                      // mpack struct for keep track about the packing
} ebv_mpack;

ebv_mpack _ebv_mpack;



// Static functions
static int8_t __ebv_iot_strlen(const char * s);
static uint8_t _ebv_mpack_getUnsignedLen(unsigned long long int i);
static uint8_t _ebv_mpack_getSignedLen(int long long i);
static bool ebv_iot_validade_response(esp_response_t * resp, uint16_t * response_len);
static ebv_ret_t ebv_iot_generic_action_run(esp_packet_t *pkg, 
                                            esp_response_t *response,
                                            uint16_t *response_len);
#if DEBUG_EN == 1
    static void ebv_iot_dump_error_code(uint16_t err);
#else
    #define ebv_iot_dump_error_code(x)      ;
#endif


/** @brief: Initialize IoT Lib
 */
void ebv_iot_init()
{
    memset(&_ebv_mpack, 0, sizeof(ebv_mpack));
    esp_com_err = EBV_ESP_COM_ERROR_NONE;
}

/** @brief: Receive an action
 *
 *  @param: (esp_response_t) *response: pointer to ESP Response structure
 *
 *  @return: (ebv_ret_t): EBV return values, could be some error code or OK
 */
ebv_ret_t ebv_iot_receive_action(esp_response_t *response)
{
    esp_packet_t pkg;
    esp_response_t resp;
    ebv_ret_t ret;
    uint16_t response_len;

    // Get Action
    DEBUG_MSG_TRACE("Receiving actions...");
    memset(pkg.data, 0, sizeof(pkg.data));
    ebv_esp_packetBuilderByArray(&pkg, ESP_CMD_GET_ACTIONS, NULL, 0);

    ret = ebv_iot_generic_action_run(&pkg, &resp, &response_len);
    if ( ret != EBV_RET_OK ) { 
        if (ret == EBV_RET_OK_WITH_ERROR) {
            response->has_error_code = true;
            memcpy( &response->response[0], &resp.response[6], 2);
        }
        return ret; 
    }
    response->has_error_code = false;

    if(response_len > ESP_CRC_LEN + ESP_FLAGS_LEN){
        response->response_len = response_len - ESP_CRC_LEN - ESP_FLAGS_LEN;
        memcpy(response->response, &resp.response[4], response->response_len);
        if(response->response_len == 1){
            return EBV_RET_NO_ACTION;
        } else {
            return EBV_RET_OK;
        }
    } else {
        response->response_len = 0;
        return EBV_RET_INV_PAYLOAD;
    }


    // Should be never reached
    return EBV_RET_ERROR;
}

bool ebv_iot_parseAction(esp_response_t *resp, ebv_action_t *action ){
    memset(action, 0x00, sizeof(ebv_action_t));
    cw_unpack_context uc;
    cw_unpack_context_init(&uc, resp->response, resp->response_len, NULL);
    cw_unpack_next(&uc);        // root array
    p("a: %d\n\r", resp->response_len);
    if( uc.item.as.array.size == 0){
        p("b\n\r");
        action->payload_len = 0;
        return true;
    }
    cw_unpack_next(&uc);        // first action array
    uint8_t nof_elemnts = uc.item.as.array.size;
    p("c: %d\n\r", nof_elemnts);
    cw_unpack_next(&uc);        // action type
    if(uc.item.type != CWP_ITEM_STR){ return false; }
    p("d\n\r");
    memcpy( action->type, uc.item.as.str.start, uc.item.as.str.length );
    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){ return false; }
    p("e\n\r");
    action->id = (uint32_t) uc.item.as.u64;
    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_MAP){ return false; }
    uint8_t * uc_map_start = uc.current - 1;
    p("f: %d\n\r", *uc_map_start);
    cw_skip_items(&uc, uc.item.as.map.size * 2);
    action->payload_len = uc.current - uc_map_start;
    p("g: %d\n\r", action->payload_len);
    memcpy(action->payload, uc_map_start, action->payload_len);
    if(nof_elemnts > 3){
        p("h\n\r");
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){ return false; }
        action->svr_lvl = (uint8_t) uc.item.as.u64;
    } else {
        p("i\n\r");
        action->svr_lvl = 0;
    }

    return true;
}

static ebv_ret_t ebv_iot_generic_action_run(esp_packet_t *pkg, 
                                            esp_response_t *resp,
                                            uint16_t *response_len)
{
    // esp_response_t resp;
    ebv_ret_t ret = ebv_esp_submit_packet(pkg, resp);
    if ( ret != EBV_RET_OK ) { return ret; }

    if( resp->command != pkg->command || resp->sop != ESP_RESPONSE_SOP_SOA_ID ){
        DEBUG_MSG_TRACE("Invalid ACK resp");
        return EBV_RET_INV_ACK;
    }

    ret = ebv_esp_query_delayed_response(resp, true);
    if ( ret != EBV_RET_OK ) { return ret; }

    if ( !ebv_iot_validade_response(resp, response_len) ) { 
        // response->has_error_code = true;
        // memcpy( &response->response[0], &resp.response[6], 2);
        return EBV_RET_OK_WITH_ERROR;
    }

    // response->has_error_code = false;
    return EBV_RET_OK;
}

/** @brief: Submit an action result
 *
 *  @param: (ebv_action_t) *a: pointer to EBV Action structure
 *  @param: (esp_response_t) *response: pointer to ESP Response structure
 *
 *  @return: (ebv_ret_t): EBV return values, could be some error code or OK
 */
ebv_ret_t ebv_iot_submit_action_result(ebv_action_t *action, esp_response_t *response)
{
    esp_packet_t pkg;
    uint16_t resp_len = 0;
    ebv_ret_t ret = EBV_RET_ERROR;

    ebv_esp_buildActionResponse(&pkg, action->id, action->response_payload, action->response_payload_size, action->result);
    ret = ebv_iot_generic_action_run(&pkg, response, &resp_len);
    if ( ret != EBV_RET_OK ) { return ret; }
    
    if(resp_len > ESP_CRC_LEN + ESP_FLAGS_LEN){
        // We should get nothing as payload
        DEBUG_MSG_TRACE("Payload len more that expecred LEN : %d", resp_len);
        return EBV_RET_ERROR;
    } else {
        response->response_len = 0;
        return EBV_RET_OK;
    }

    // Should never reache this point
    return EBV_RET_ERROR;
}

ebv_ret_t ebv_iot_submitGenericActionResult(ebv_action_t *action, esp_response_t *response){
    action->response_payload = _ebv_mpack.buff;
    action->response_payload_size = _ebv_mpack.c.current - _ebv_mpack.c.start;
    // Update the final map size
    uint8_t i = 0;
    while(i < action->response_payload_size){
        if(action->response_payload[i] == 0x81){
            action->response_payload[i] = 0x80 + _ebv_mpack.elements;
            i = action->response_payload_size;
        }
        i++;
    }
    ebv_ret_t ret;
    if( i > action->response_payload_size){     // Set the right map size was successfull
        ret = ebv_iot_submit_action_result(action, response);
    } else {            // Set the right map size was unsuccessfull
        ret = EBV_RET_ERROR;
    }
    _ebv_mpack.isBufferReady = false;
    return ret;
}

bool ebv_iot_submitEvent(ebv_iot_event *e){
#if DEBUG_EN == 1
    DEBUG_MSG_TRACE("Submitting event, len : %d", e->len);
    uint8_t i;
    for(i = 0; i < e->len; i++){
        if((i % 8) == 0){DEBUG_MSG("\n\r");}
        DEBUG_MSG("%X ", e->body[i]);
    }
    DEBUG_MSG("\n\r");
#endif
    esp_packet_t pkg;
    ebv_esp_packetBuilderByArray(&pkg, ESP_CMD_PUT_EVENTS, e->body, e->len);
    FAIL_IF_NOT(waitForDevice(), EBV_ESP_COM_ERROR_BUSY_TIMEOUT);

    ebv_esp_sendCommand(&pkg);
    esp_response_t response;
    ebv_delay(10);
    FAIL_IF_NOT(ebv_esp_receiveResponse(&pkg, &response), EBV_ESP_COM_ERROR_ACK_TIMEOUT);

    ebv_delay(20);
    FAIL_IF_NOT(waitForDevice(), EBV_ESP_COM_ERROR_BUSY_TIMEOUT);
    FAIL_IF_NOT(wait_response_available(), EBV_ESP_COM_ERROR_RESPONSE_TIMEOUT);

    ebv_esp_packetBuilderByArray(&pkg, ESP_CMD_READ_DELAYED_RESP, NULL, 0);
    ebv_esp_sendCommand(&pkg);
    ebv_delay(20);
    FAIL_IF_NOT(waitForDevice(), EBV_ESP_COM_ERROR_BUSY_TIMEOUT);
    FAIL_IF_NOT(ebv_esp_receiveResponse(&pkg, &response), EBV_ESP_COM_ERROR_RESPONSE_TIMEOUT);
    FAIL_IF_NOT(
        (ebv_esp_resp_res_t)ebv_esp_eval_delayed_resp(&response, ESP_CMD_PUT_EVENTS) == EBV_ESP_RESP_RES_OK,
        EBV_ESP_COM_ERROR_RESPONSE_INVALID);

    return true;
}

bool ebv_iot_submitGenericEvent(){
    ebv_iot_event e;
    e.body = _ebv_mpack.buff;
    e.len = _ebv_mpack.c.current - _ebv_mpack.c.start;
    // e.len = _ebv_mpack.c.current - _ebv_mpack.c.start;
    // Update the final map size
    uint8_t i = 3; // We know that the first 3 item is static
    while(i < e.len){
        if(e.body[i] == 0x81){
            e.body[i] = 0x80 + _ebv_mpack.elements;
            i = e.len;
        }
        i++;
    }

    bool ret;
    if( i > e.len){     // Set the right map size was successfull
        ret = ebv_iot_submitEvent(&e);
    } else {            // Set the right map size was unsuccessfull
        ret = false;
    }
    _ebv_mpack.isBufferReady = false;
    return ret;
}

bool ebv_iot_initGenericEvent(const char * evnt_type){
    memset(_ebv_mpack.buff, 0, sizeof(_ebv_mpack.buff));
    _ebv_mpack.size = sizeof(_ebv_mpack.buff);
    _ebv_mpack.elements = 0;
    _ebv_mpack.isBufferReady = true;
    cw_pack_context_init(&_ebv_mpack.c, _ebv_mpack.buff, sizeof(_ebv_mpack.buff), NULL);
    cw_pack_array_size(&_ebv_mpack.c, 1);
    cw_pack_array_size(&_ebv_mpack.c, 2);
    // TODO Need to check if the next string fit in the mem or not
    cw_pack_str(&_ebv_mpack.c, evnt_type, __ebv_iot_strlen(evnt_type));
    cw_pack_map_size(&_ebv_mpack.c, 1);     // Preparing it for only one element
    return true;
}

bool ebv_iot_initGenericResponse(){
    memset(_ebv_mpack.buff, 0, sizeof(_ebv_mpack.buff));
    _ebv_mpack.size = sizeof(_ebv_mpack.buff);
    _ebv_mpack.elements = 0;
    _ebv_mpack.isBufferReady = true;
    cw_pack_context_init(&_ebv_mpack.c, _ebv_mpack.buff, sizeof(_ebv_mpack.buff), NULL);
    cw_pack_map_size(&_ebv_mpack.c, 1);     // Preparing it for only one element
    return true;
}

bool _ebv_iot_addUnsignedPayload(const char * k, unsigned int v){
    uint8_t k_len = __ebv_iot_strlen(k);
    cw_pack_str(&_ebv_mpack.c, k, k_len);
    cw_pack_unsigned(&_ebv_mpack.c, v);
    _ebv_mpack.elements++;
    return true;
}

bool _ebv_iot_addSignedPayload(const char * k, int v){
    uint8_t k_len = __ebv_iot_strlen(k);
    cw_pack_str(&_ebv_mpack.c, k, k_len);
    cw_pack_signed(&_ebv_mpack.c, v);
    _ebv_mpack.elements++;
    return true;
}

bool _ebv_iot_addFloatPayload(const char * k, float v){
    uint8_t k_len = __ebv_iot_strlen(k);
    cw_pack_str(&_ebv_mpack.c, k, k_len);
    cw_pack_float(&_ebv_mpack.c, v);
    _ebv_mpack.elements++;
    return true;
}

bool _ebv_iot_addDoublePayload(const char * k, double v){
    uint8_t k_len = __ebv_iot_strlen(k);
    cw_pack_str(&_ebv_mpack.c, k, k_len);
    cw_pack_double(&_ebv_mpack.c, v);
    _ebv_mpack.elements++;
    return true;
}

bool _ebv_iot_addStringPayload(const char * k, const char * v){
    uint8_t k_len = __ebv_iot_strlen(k);
    uint8_t v_len = __ebv_iot_strlen(v);
    // TODO: Check remaining space in buffer 
    cw_pack_str(&_ebv_mpack.c, k, k_len);
    cw_pack_str(&_ebv_mpack.c, v, v_len);
    _ebv_mpack.elements++;
    return true;
}

bool _ebv_iot_addCharPayload(const char * k, const char v){
    uint8_t k_len = __ebv_iot_strlen(k);
    cw_pack_str(&_ebv_mpack.c, k, k_len);
    cw_pack_str(&_ebv_mpack.c, &v, 1);
    _ebv_mpack.elements++;
    return true;
}

static int8_t __ebv_iot_strlen(const char * s){
    uint8_t c = 0;
    while( s[c] && c < EBV_STRLEN_MAXLEN){
        c++;
    }
    return c >= EBV_STRLEN_MAXLEN ? -1 : c;
}

static uint8_t _ebv_mpack_getUnsignedLen(unsigned long long int i){
    if (i < 128){
        return 0;
    }
    if (i < 256){
        return 1;
    }
    if (i < 0x10000L){
        return 2;
    }
    if (i < 0x100000000LL) {
        return 4;
    }
    return 8;
}

static uint8_t _ebv_mpack_getSignedLen(long long int i){
    if (i >127)
    {
        if (i < 256){
            return 1;
        }
        if (i < 0x10000L){
            return 2;
        }
        if (i < 0x100000000LL){
            return 4;
        }
        return 8;
    }
    
    if (i >= -32){
        return 0;
    }
    if (i >= -128){
        return 1;
    }
    if (i >= -32768){
        return 2;
    }
    if (i >= (int64_t)0xffffffff80000000LL){
        return 4;
    }
    return 8;
}

ebv_esp_com_error_t ebv_iot_get_last_error_code(){
    return esp_com_err;
}

static bool ebv_iot_validade_response(esp_response_t * resp, uint16_t * response_len)
{
    uint16_t payload_id   = (uint16_t) resp->response[4] | (uint16_t) resp->response[5] << 8; // This is for identifying the payload in case of error
    * response_len = (uint16_t) resp->response[2] | (uint16_t) resp->response[3] << 8;
    if((*response_len) == 4 && payload_id == ESP_DL_PAYLOAD_KIND_ERROR) {
        // We got an error, let's see which
        DEBUG_MSG_TRACE("Pkg received: LEN: %X P_ID: %X", (*response_len), payload_id);
        DEBUG_MSG_TRACE("Response result: ");
        ebv_iot_dump_error_code((uint16_t)((uint16_t) resp->response[6] | (uint16_t) resp->response[7] << 8));
        return false;
    } else {
        DEBUG_MSG_TRACE("Pkg received: SOP: %x, CMD: %x, LEN: %d",resp->response[0], resp->response[1], (*response_len));
        return true;
    }
}

#if DEBUG_EN == 1
static void ebv_iot_dump_error_code(uint16_t err)
{
    switch (err) {
        case ESP_ERR_INVALID_CMD_DATA:
            DEBUG_MSG_TRACE("Invalid data");
            break;
        case ESP_ERR_INVALID_CMD_ID:
            DEBUG_MSG_TRACE("Unkown CMD");
            break;
        default:
            DEBUG_MSG_TRACE("Unknown error");
            break;
    }
}
#endif

#if EBV_COM_ERROR_DUMP_EN == 1
void ebv_iot_dump_last_error(){
    p("\n\r%s\n\r", xstr(esp_com_err));
}
#else
#define ebv_iot_dump_last_error()
#endif

#ifdef __cplusplus
    void ebv_iot_addGenericPayload(const char * key, unsigned int value){
        _ebv_iot_addUnsignedPayload(key, value);
    }
    void ebv_iot_addGenericPayload(const char * key, int value){
        _ebv_iot_addSignedPayload(key, value);
    }
    void ebv_iot_addGenericPayload(const char * key, float value){
        _ebv_iot_addFloatPayload(key, value);
    }
    void ebv_iot_addGenericPayload(const char * key, double value){
        _ebv_iot_addDoublePayload(key, value);
    }
    void ebv_iot_addGenericPayload(const char * key, const char * value){
        _ebv_iot_addStringPayload(key, value);
    }
    void ebv_iot_addGenericPayload(const char * key, const char value){
        _ebv_iot_addCharPayload(key, value);
    }
#endif
