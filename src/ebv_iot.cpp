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

#include <string.h>
#include <stdlib.h>

#define LOW 0
#define  HI 1

#define UNUSED(...) (void)(__VA_ARGS__)

#define EBV_MPACK_DEFAULT_SIZE          32
#define EBV_STRLEN_MAXLEN               64
#define EBV_MPACK_DATATYPE_HEADER        1
#define EBV_MPACK_FLOAT_LEN              4
#define EBV_MPACK_DOUBLE_LEN             8

typedef struct{
    uint8_t *buff;          // This is the mapack buffer
    uint8_t size;           // The current size of the buffer
    uint8_t elements;       // Count of the elements in the mpacked content
    bool isBufferReady;     // Flag for indicating the budder status
    cw_pack_context c;      // mpack struct for keep track about the packing
} ebv_mpack;

ebv_mpack _ebv_mpack = {0};



// Static functions
static int8_t __ebv_iot_strlen(char * s);
static bool _ebv_iot_allocateBufferFor(uint8_t n_item_size);
static uint8_t _ebv_mpack_getUnsignedLen(unsigned long long int i);
static uint8_t _ebv_mpack_getSignedLen(int long long i);

void ebv_iot_receiveAction(esp_response_t *response){
    esp_packet_t pkg;
    esp_response_t resp;
    // Get Action
    // p("Submitting GET ACTION...\n\r");
    ebv_esp_packetBuilderByArray(&pkg, ESP_CMD_GET_ACTIONS, NULL, 0);
    if ( !waitForDevice() ){ return; }
    ebv_esp_sendCommand(&pkg);
    ebv_delay(10);
    // p("Reading response...\n\r");
    ebv_esp_receiveResponse(&pkg, &resp);
    if(resp.command == ESP_CMD_GET_ACTIONS && resp.sop == ESP_RESPONSE_SOP_SOA_ID){
        // p("ACK Response received successfully\n\r");
    } else {
       // p("ACK Response is wrong\n\r");
    }
    //IRQ going to low because the GET_ACTIONS command generated a delayed response
    if ( !waitForResponse() ){ return; }
    if ( !waitForDevice() ){ return; }
    if(ebv_esp_gpio_readIRQ() == LOW){    // Check it again
        // Read delayed response
        ebv_esp_packetBuilderByArray(&pkg, ESP_CMD_READ_DELAYED_RESP, NULL, 0);
        ebv_esp_sendCommand(&pkg);
        while( !ebv_esp_gpio_readReady() );   // Wait until READY goes HI
        ebv_esp_receiveResponse(&pkg, &resp);
        // Parsing respose, this is will be the delayed response header
        uint8_t trigger_cmd = resp.response[1];                                                                 // This is the cmd which created the delayed response
        uint16_t response_len = (uint16_t) resp.response[2] | (uint16_t) resp.response[3] << 8;
        uint16_t payload_id = (uint16_t) resp.response[4] | (uint16_t) resp.response[5] << 8;                   // This is for identifying the payload
        if(response_len == 4 && payload_id == ESP_DL_PAYLOAD_KIND_ERROR){                                       // We got an error, let it see what kind of
            // p("Pkg received: TR_CMD: 0x%x LEN: 0x%x P_ID: 0x%x\n\r", trigger_cmd, response_len, payload_id);
            // p("Response result: ");
            uint16_t err_code = (uint16_t) resp.response[6] | (uint16_t) resp.response[7] << 8;
            switch (err_code){
                case ESP_ERR_INVALID_CMD_DATA:
                    // p("Invalid data for CMD: 0x%x\n\r", trigger_cmd);
                    break;
                case ESP_ERR_INVALID_CMD_ID:
                    // p("Unkown CMD: 0x%x\n\r", trigger_cmd);
                    break;
                default:
                    // p("Not Handled\n\r");
                    break;
            }
        } else {
            // p("\n\rPkg received: SOP: 0x%x, RESP_CMD: 0x%x, LEN: %d\n\r",resp.response[0], resp.response[1], response_len);
            // ebv_esp_dumpPayload(&resp.response[4], response_len - ESP_CRC_LEN - ESP_FLAGS_LEN);
            if(payload_id > ESP_CRC_LEN + ESP_FLAGS_LEN){
                response->response_len = response_len - ESP_CRC_LEN - ESP_FLAGS_LEN;
                response->response = (uint8_t *) malloc(response->response_len);
                memcpy(response->response, &resp.response[4], response->response_len);
            } else {
                response->response_len = 0;
            }
            free(resp.response);
        }
    }
}

bool ebv_iot_parseAction(esp_response_t *resp, ebv_action_t *action ){
    memset(action, 0x00, sizeof(ebv_action_t));
    cw_unpack_context uc;
    cw_unpack_context_init(&uc, resp->response, resp->response_len, NULL);
    cw_unpack_next(&uc);        // root array
    if( uc.item.as.array.size == 0){
        action->payload_len = 0;
        return true;
    }
    cw_unpack_next(&uc);        // first action array
    uint8_t nof_elemnts = uc.item.as.array.size;
    cw_unpack_next(&uc);        // action type
    if(uc.item.type != CWP_ITEM_STR){ return false; }
    memcpy( action->type, uc.item.as.str.start, uc.item.as.str.length );
    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){ return false; }
    action->id = (uint32_t) uc.item.as.u64;
    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_MAP){ return false; }
    uint8_t * uc_map_start = uc.current - 1;
    cw_skip_items(&uc, uc.item.as.map.size * 2);
    action->payload_len = uc.current - uc_map_start;
    memcpy(action->payload, uc_map_start, action->payload_len);
    if(nof_elemnts > 3){
        cw_unpack_next(&uc);
        if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){ return false; }
        action->svr_lvl = (uint8_t) uc.item.as.u64;
    } else {
        action->svr_lvl = 0;
    }

    return true;
}

bool ebv_iot_submitActionResult(ebv_action_t *a, esp_response_t *response){
    esp_packet_t pkg;
    esp_response_t resp;
    ebv_esp_buildActionResponse(&pkg, a->id, a->response_payload, a->response_payload_size, a->result);
    if ( !waitForDevice() ){ return false; }
    ebv_esp_sendCommand(&pkg);
    ebv_delay(10);
    p("Reading response...");
    ebv_esp_receiveResponse(&pkg, &resp);
    if(resp.command == ESP_CMD_GET_ACTIONS && resp.sop == ESP_RESPONSE_SOP_SOA_ID){
        // p("ACK Response received successfully");
    } else {
       return false;
    }
    //IRQ going to low because the GET_ACTIONS command generated a delayed response
    if ( !waitForResponse() ){ return false; }
    if ( !waitForDevice() ){ return false; }
    if(ebv_esp_gpio_readIRQ() == LOW){    // Check it again
        // Read delayed response
        ebv_esp_packetBuilderByArray(&pkg, ESP_CMD_READ_DELAYED_RESP, NULL, 0);
        ebv_esp_sendCommand(&pkg);
        while( ebv_esp_gpio_readReady() );   // Wait until READY goes HI
        ebv_esp_receiveResponse(&pkg, &resp);
        // Parsing respose, this is will be the delayed response header
        uint16_t response_len = (uint16_t) resp.response[2] | (uint16_t) resp.response[3] << 8;
        uint16_t payload_id = (uint16_t) resp.response[4] | (uint16_t) resp.response[5] << 8;                   // This is for identifying the payload
        if(response_len == 4 && payload_id == ESP_DL_PAYLOAD_KIND_ERROR){                                       // We got an error, let it see what kind of
            // p("Pkg received: LEN: 0x%x P_ID: 0x%x\n\r", response_len, payload_id);
            // p("Response result: ");
            uint16_t err_code = (uint16_t) resp.response[6] | (uint16_t) resp.response[7] << 8;
            switch (err_code){
                case ESP_ERR_INVALID_CMD_DATA:
                    // // p("Invalid data for CMD: 0x%x\n\r", trigger_cmd);
                    break;
                case ESP_ERR_INVALID_CMD_ID:
                    // // p("Unkown CMD: 0x%x\n\r", trigger_cmd);
                    break;
                default:
                    // Serial.print("Not Handled\n\r");
                    break;
            }
        } else {
            // p("\n\rPkg received: SOP: 0x%x, RESP_CMD: 0x%x, LEN: %d\n\r",resp.response[0], resp.response[1], response_len);
            // ebv_esp_dumpPayload(&resp.response[4], payload_len - ESP_CRC_LEN - ESP_FLAGS_LEN);
            if(payload_id > ESP_CRC_LEN + ESP_FLAGS_LEN){
                response->response_len = payload_id - ESP_CRC_LEN - ESP_FLAGS_LEN;
                response->response = (uint8_t *) malloc(response->response_len);
                memcpy(response->response, &resp.response[4], response->response_len);
            } else {
                response->response_len = 0;
            }
            free(resp.response);
        }
    }
    return true;
}

bool ebv_iot_submitGenericActionResult(ebv_action_t *a, esp_response_t *response){
    a->response_payload = _ebv_mpack.buff;
    a->response_payload_size = _ebv_mpack.c.current - _ebv_mpack.c.start;
    // Update the final map size
    uint8_t i = 0;
    while(i < a->response_payload_size){
        if(a->response_payload[i] == 0x81){
            a->response_payload[i] = 0x80 + _ebv_mpack.elements;
            i = a->response_payload_size;
        }
        i++;
    }
    bool ret;
    if( i > a->response_payload_size){     // Set the right map size was successfull
        ret = ebv_iot_submitActionResult(a, response);
    } else {            // Set the right map size was unsuccessfull
        ret = false;
    }
    free(_ebv_mpack.buff);
    _ebv_mpack.isBufferReady = false;
    return ret;
}

bool ebv_iot_submitEvent(ebv_iot_event *e){
    esp_packet_t pkg;
    ebv_esp_packetBuilderByArray(&pkg, ESP_CMD_PUT_EVENTS, e->body, e->len);
    if( !waitForDevice() ){
        // p("Timeout, waitForDevice");
        return false;
    }
    ebv_esp_sendCommand(&pkg);
    esp_response_t response;
    ebv_delay(10);
    ebv_esp_receiveResponse(&pkg, &response);

    ebv_delay(20);
    if( !waitForDevice() ){
        // p("Timeout, waitForDevice");
        return false;
    }
    while( !isResponseAvailable() );
    ebv_esp_packetBuilderByArray(&pkg, ESP_CMD_READ_DELAYED_RESP, NULL, 0);
    ebv_esp_sendCommand(&pkg);
    do{
        ebv_delay(5);
    } while ( isDeviceBusy() );
    ebv_esp_receiveResponse(&pkg, &response);
    return true;
}

bool ebv_iot_submitGenericEvent(){
    ebv_iot_event e;
    e.body = _ebv_mpack.buff;
    e.len = _ebv_mpack.c.current - _ebv_mpack.c.start;
    // Update the final map size
    uint8_t i = 3; // We know that the first 3 item will be static
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
    free(_ebv_mpack.buff);
    _ebv_mpack.isBufferReady = false;
    return ret;
}

bool ebv_iot_initGenericEvent(char * evnt_type){
    if(_ebv_mpack.buff){
        free(_ebv_mpack.buff);
    }
    _ebv_mpack.buff = (uint8_t *) malloc(EBV_MPACK_DEFAULT_SIZE);
    if (!_ebv_mpack.buff){
        return false;
    }
    _ebv_mpack.size = EBV_MPACK_DEFAULT_SIZE;
    _ebv_mpack.elements = 0;
    _ebv_mpack.isBufferReady = true;
    cw_pack_context_init(&_ebv_mpack.c, _ebv_mpack.buff, EBV_MPACK_DEFAULT_SIZE, NULL);
    cw_pack_array_size(&_ebv_mpack.c, 1);
    cw_pack_array_size(&_ebv_mpack.c, 2);
    // TODO Need to check if the next string fit in the mem or not
    cw_pack_str(&_ebv_mpack.c, evnt_type, __ebv_iot_strlen(evnt_type));
    cw_pack_map_size(&_ebv_mpack.c, 1);     // Preparing it for only one element
    return true;
}

bool ebv_iot_initGenericResponse(){
    if(_ebv_mpack.buff){
        free(_ebv_mpack.buff);
    }
    _ebv_mpack.buff = (uint8_t *) malloc(EBV_MPACK_DEFAULT_SIZE);
    if (!_ebv_mpack.buff){
        return false;
    }
    _ebv_mpack.size = EBV_MPACK_DEFAULT_SIZE;
    _ebv_mpack.elements = 0;
    _ebv_mpack.isBufferReady = true;
    cw_pack_context_init(&_ebv_mpack.c, _ebv_mpack.buff, EBV_MPACK_DEFAULT_SIZE, NULL);
    cw_pack_map_size(&_ebv_mpack.c, 1);     // Preparing it for only one element
    return true;
}

bool _ebv_iot_addUnsignedPayload(char * k, unsigned int v){
    uint8_t k_len = __ebv_iot_strlen(k);
    if( !_ebv_iot_allocateBufferFor(k_len + _ebv_mpack_getUnsignedLen(v) + 2 * EBV_MPACK_DATATYPE_HEADER)){
        return false;
    }
    cw_pack_str(&_ebv_mpack.c, k, k_len);
    cw_pack_unsigned(&_ebv_mpack.c, v);
    _ebv_mpack.elements++;
    return true;
}

bool _ebv_iot_addSignedPayload(char * k, int v){
    uint8_t k_len = __ebv_iot_strlen(k);
    if( !_ebv_iot_allocateBufferFor(k_len + _ebv_mpack_getSignedLen(v) + 2 * EBV_MPACK_DATATYPE_HEADER)){
        return false;
    }
    cw_pack_str(&_ebv_mpack.c, k, k_len);
    cw_pack_signed(&_ebv_mpack.c, v);
    _ebv_mpack.elements++;
    return true;
}

bool _ebv_iot_addFloatPayload(char * k, float v){
    uint8_t k_len = __ebv_iot_strlen(k);
    if( !_ebv_iot_allocateBufferFor(k_len + EBV_MPACK_FLOAT_LEN + 2 * EBV_MPACK_DATATYPE_HEADER)){
        return false;
    }
    cw_pack_str(&_ebv_mpack.c, k, k_len);
    cw_pack_float(&_ebv_mpack.c, v);
    _ebv_mpack.elements++;
    return true;
}

bool _ebv_iot_addDoublePayload(char * k, double v){
    uint8_t k_len = __ebv_iot_strlen(k);
    if( !_ebv_iot_allocateBufferFor(k_len + EBV_MPACK_DOUBLE_LEN + 2 * EBV_MPACK_DATATYPE_HEADER)){
        return false;
    }
    cw_pack_str(&_ebv_mpack.c, k, k_len);
    cw_pack_double(&_ebv_mpack.c, v);
    _ebv_mpack.elements++;
    return true;
}

bool _ebv_iot_addStringPayload(char * k, char * v){
    uint8_t k_len = __ebv_iot_strlen(k);
    uint8_t v_len = __ebv_iot_strlen(v);
    if( !_ebv_iot_allocateBufferFor(k_len + v_len + 2 * EBV_MPACK_DATATYPE_HEADER)){
        return false;
    }
    // TODO: Check remaining space in buffer 
    cw_pack_str(&_ebv_mpack.c, k, k_len);
    cw_pack_str(&_ebv_mpack.c, v, v_len);
    _ebv_mpack.elements++;
    return true;
}

bool _ebv_iot_addCharPayload(char * k, char v){
    uint8_t k_len = __ebv_iot_strlen(k);
     if( !_ebv_iot_allocateBufferFor(k_len + 1 + 2 * EBV_MPACK_DATATYPE_HEADER)){
        return false;
    }
    cw_pack_str(&_ebv_mpack.c, k, k_len);
    cw_pack_str(&_ebv_mpack.c, &v, 1);
    _ebv_mpack.elements++;
    return true;
}

static int8_t __ebv_iot_strlen(char * s){
    uint8_t c = 0;
    while( s[c] && c < EBV_STRLEN_MAXLEN){
        c++;
    }
    return c >= EBV_STRLEN_MAXLEN ? -1 : c;
}

static bool _ebv_iot_allocateBufferFor(uint8_t n_item_size){
    // Check if we need allocate more memory or not
    uint16_t free_space  = _ebv_mpack.c.end - _ebv_mpack.c.current; 
    if(n_item_size > free_space ){
        _ebv_mpack.size += n_item_size - free_space;
        _ebv_mpack.buff = (uint8_t *) realloc(_ebv_mpack.buff, _ebv_mpack.size);
        if(!_ebv_mpack.buff){
            return false;
        }
        _ebv_mpack.c.end = _ebv_mpack.c.start + _ebv_mpack.size;
    }

    return true;
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

#ifdef __cplusplus
    void ebv_iot_addGenericPayload(char * key, unsigned int value){
        _ebv_iot_addUnsignedPayload(key, value);
    }
    void ebv_iot_addGenericPayload(char * key, int value){
        _ebv_iot_addSignedPayload(key, value);
    }
    void ebv_iot_addGenericPayload(char * key, float value){
        _ebv_iot_addFloatPayload(key, value);
    }
    void ebv_iot_addGenericPayload(char * key, double value){
        _ebv_iot_addDoublePayload(key, value);
    }
    void ebv_iot_addGenericPayload(char * key, char * value){
        _ebv_iot_addStringPayload(key, value);
    }
    void ebv_iot_addGenericPayload(char * key, char value){
        _ebv_iot_addCharPayload(key, value);
    }
#endif