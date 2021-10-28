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

#include "ebv_i2c.h"
#include "ebv_esp_gpio.h"
#include "ebv_esp.h"
#include "ebv_delay.h"
#include "print_serial.h"

#include <extcwpack.h>
#include <stdlib.h>
#include <string.h>

#define PKG_CRC_LEN 4
#define USE_CRC 0

// You can use this macro to kick the debuging in 
#define TESTSUITE_DEBUG 0

uint8_t DEVICE_ADDRESS = DEFAULT_DEVICE_ADDRESS;

uint32_t ebv_esp_generateCrc32( uint8_t *data, uint8_t len );

void ebv_esp_setDeviceAddress(uint8_t addr){
    DEVICE_ADDRESS = addr;
}

void ebv_esp_packetBuilderByArray(esp_packet_t *pkg, uint8_t command, uint8_t* data, uint8_t data_len){
    pkg->command = command;
    if(USE_CRC){
        pkg->flags = 0;
    } else {
        pkg->flags = 1;
    }
    if(data_len){
        pkg->data = (uint8_t *) malloc(data_len);
        memcpy(pkg->data, data, data_len);
        pkg->len = data_len + PKG_CRC_LEN;
    } else {
        pkg->data = NULL;
        pkg->len = 0;
    }
    if(USE_CRC){
        pkg->crc32 = ebv_esp_generateCrc32(pkg->data, data_len);
    } else {
        pkg->crc32 = 0;
    }
}

void ebv_esp_sendCommand(esp_packet_t *pkg){
    switch(pkg->command){
        case ESP_CMD_READ_DELAYED_RESP:
        case ESP_CMD_GET_ACTIONS:
            ebv_i2c_I2cBeginTransaction(DEVICE_ADDRESS);
            ebv_i2c_I2cWrite(pkg->command);
            // Wire.write(0x00);
            ebv_i2c_I2cFinishTransaction();
            break;
        default:
            ebv_i2c_I2cBeginTransaction(DEVICE_ADDRESS);
            ebv_i2c_I2cWrite(pkg->command);
            ebv_i2c_I2cWrite(pkg->flags);
            ebv_i2c_I2cWrite( (uint8_t) pkg->len);
            ebv_i2c_I2cWrite( (uint8_t) pkg->len >> 8);
            uint8_t payload_len = pkg->len;
            payload_len -= PKG_CRC_LEN;
            if(pkg->len > 4){
                for(uint8_t i = 0; i < payload_len; i++){
                    ebv_i2c_I2cWrite(pkg->data[i]);
                }
            }
            ebv_i2c_I2cWrite( (uint8_t) pkg->crc32);
            ebv_i2c_I2cWrite( (uint8_t) pkg->crc32 >> 8);
            ebv_i2c_I2cWrite( (uint8_t) pkg->crc32 >> 16);
            ebv_i2c_I2cWrite( (uint8_t) pkg->crc32 >> 24);
            ebv_i2c_I2cFinishTransaction();
            break;
    }
}

void ebv_esp_receiveResponse(esp_packet_t *pkg, esp_response_t *resp){
    switch(pkg->command){
        case ESP_CMD_READ_DELAYED_RESP:
            // Read response packet header for get the packet length
            ebv_i2c_I2cRequest(DEVICE_ADDRESS, 4);
            while(ebv_i2c_I2cAvailable() >= 4){
                resp->sop = ebv_i2c_I2cRead();
                resp->command = ebv_i2c_I2cRead();
                resp->len = ebv_i2c_I2cRead();
                resp->len |= ((uint16_t) ebv_i2c_I2cRead()) << 8;
            }
            #if (TESTSUITE_DEBUG == 1)
            // p("PKG Header Received: SOP:%x CMD:%x LEN:%d\n\r", resp->sop, resp->command, resp->len);
            #endif
            if ( resp->len > EBV_ESP_MAX_PAYLOAD_SIZE){
                return;
            }
            if(resp->len){
                // Read the whole response packet
                uint8_t pkg_len = ESP_DELAYED_RESPONSE_HEADER_LEN + resp->len;
                ebv_i2c_I2cRequest(DEVICE_ADDRESS, pkg_len);
                if(resp->response){ free(resp->response); }
                resp->response = (uint8_t *) malloc(resp->len);
                while(ebv_i2c_I2cAvailable() >= pkg_len){
                    resp->sop = ebv_i2c_I2cRead();
                    resp->command = ebv_i2c_I2cRead();
                    resp->len = ebv_i2c_I2cRead();
                    resp->len |= ((uint16_t) ebv_i2c_I2cRead()) << 8;
                    for(uint8_t i = 0; i< resp->len; i++){
                        resp->response[i] = ebv_i2c_I2cRead();
                    }
                }
            }
            #if (TESTSUITE_DEBUG == 1)
            // p("PKG Received: SOP:%x CMD:%x LEN:%d\n\r", resp->sop, resp->command, resp->len);
            for(uint8_t i = 0; i < resp->len; i++){
                if( !(i % 8)){ Serial.print("\n\r"); }
                // p("0x%x ",resp->response[i]);
            }
            #endif
            break;
        default:
            // ACK PKG in case of unknown packet
            ebv_i2c_I2cRequest(DEVICE_ADDRESS, 2);
            while(ebv_i2c_I2cAvailable() >= 2){
                resp->sop = ebv_i2c_I2cRead();
                resp->command = ebv_i2c_I2cRead();
            }
            #if (TESTSUITE_DEBUG == 1)
            // p("Response SOP: 0x%x CMD:0x%x\n\r", resp->sop, resp->command);
            #endif
            break;
    }
}

// MSGPack decoder
 #if (TESTSUITE_DEBUG == 1)
void ebv_esp_dumpPayload(uint8_t *payload, uint8_t payload_len){
    uint8_t parent_kind;
    uint8_t nested_level[10] = {0};
    int8_t nested_level_index = -1;
    uint8_t nof_childs;
    cw_unpack_context uc;
    cw_unpack_context_init(&uc, payload, payload_len, 0);
    do {
        cw_unpack_next(&uc);
        switch( uc.item.type ){
            case CWP_ITEM_ARRAY:
                Serial.print("[");
                nested_level_index++;
                nested_level[nested_level_index] = uc.item.as.array.size + 1;
                break;
            case CWP_ITEM_MAP:
                Serial.print("{");
                nested_level_index++;
                nested_level[nested_level_index] = ((uc.item.as.map.size * 2) + 1) | 0xC0;      // Use the upper 2 bits to store the type
                break;
            case CWP_ITEM_POSITIVE_INTEGER:
                // p("%d", uc.item.as.u64);
                break;
            case CWP_ITEM_STR: {
                uint8_t str_len = uc.item.as.str.length;
                for (uint8_t i = 0; i < str_len; i++){
                    Serial.print( ((char *) (uc.item.as.str.start))[i] );
                }
                break;
            }
            default:
                break;
        }
        // Print the termination, based on obj kind
        if( (uc.item.type == CWP_ITEM_ARRAY) || (uc.item.type == CWP_ITEM_MAP) ){
            if(uc.item.as.array.size){
                Serial.print("\n\r");
                 for (uint8_t i = 0; i <= nested_level_index; i++){
                    Serial.print("   ");
                }
            }
        } else {
            if(nested_level[nested_level_index] & (1 << 6)){
                Serial.print(" : ");
                nested_level[nested_level_index] &= ~(1 << 6);
            } else {
                if((nested_level[nested_level_index] & 0x3F) <= 1){
                    Serial.print("\n\r");
                    for (uint8_t i = 0; i <= nested_level_index - 1; i++){
                       Serial.print("   ");
                    }
                } else {
                    Serial.print(",\n\r");
                    for (uint8_t i = 0; i <= nested_level_index; i++){
                       Serial.print("   ");
                    }
                }
            }
        }

        // We need to decrease the child elemnts counter if that is not an array or map, a.k.a some kind of element
        volatile uint8_t nested_lvl_counter = 0;
        if( nested_level[nested_level_index] & 0x3F ){
            nested_lvl_counter = (nested_level[nested_level_index] & 0x3F) - 1;
        }
        nested_level[nested_level_index] &= 0xC0;                                    
        nested_level[nested_level_index] |= nested_lvl_counter;
        // In case of array or map, we also need to decrement the parent childrens counter
        if(nested_level_index && ( (uc.item.type == CWP_ITEM_ARRAY) || (uc.item.type == CWP_ITEM_MAP) )){
            volatile uint8_t nested_lvl_counter = 0;
            if( nested_level[nested_level_index - 1] & 0x3F ){
                nested_lvl_counter = (nested_level[nested_level_index - 1] & 0x3F) - 1;
            }
            nested_level[nested_level_index - 1] &= 0xC0;                                   
            nested_level[nested_level_index - 1] |= nested_lvl_counter;
        }
        if( nested_lvl_counter <= 0){
            // We finished with this nested level, check the type, and set a termination character
            if(nested_level[nested_level_index] & 0x80){
                // Check if this is the last one
                if((nested_level[nested_level_index - 1] & 0x3F) < 1){
                    // p("}\n\r");
                } else {
                    // p("},\n\r");
                }
            } else {
                if((nested_level[nested_level_index - 1] & 0x3F) < 1){
                    // p("]");
                } else {
                    // p("],\n\r");
                }
            }
            
            // go upper in the tree, and set pritty print dept
            nested_level_index--;
            if(nested_level_index || nested_level[nested_level_index]){
                for (uint8_t i = 0; i <= nested_level_index; i++){
                    // p("   ");
                }
            } else {
                // This case if for the upper tree element closing
                // p("\n\r]\n\r");
            }
        }
    } while( uc.item.type != CWP_NOT_AN_ITEM);
}
#else
void ebv_esp_dumpPayload(uint8_t *payload, uint8_t payload_len){
    return;
}
#endif

uint32_t ebv_esp_generateCrc32( uint8_t *data, uint8_t len ){
    return 0;
}

uint32_t ebv_esp_getActionId( uint8_t *mpack_action_payload, uint8_t len ){
    cw_unpack_context uc;
    cw_unpack_context_init(&uc, mpack_action_payload, len, NULL);
    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_ARRAY){
        return 0;
    }
    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_STR){
        return 0;
    }
    cw_unpack_next(&uc);
    if(uc.item.type != CWP_ITEM_POSITIVE_INTEGER){
        return 0;
    }
    return uc.item.as.u64;
}

void ebv_esp_buildActionResponse(esp_packet_t *pkg, uint32_t action_id, uint8_t *mpack_response_details, uint8_t response_details_len, bool isActionSucceed){
    uint8_t mpack_buff[EBV_TMP_BUFF_MAXSIZE];
    cw_pack_context c;
    cw_pack_context_init(&c, mpack_buff, sizeof(mpack_buff), NULL);
    cw_pack_array_size(&c, 1);
    // format of an action response : [<id> ,{<data_object>}, (<rc>)]
    cw_pack_array_size(&c, 3);
    cw_pack_unsigned(&c, action_id);
    if(response_details_len){
        cw_pack_insert(&c, mpack_response_details, response_details_len);
    } else {
        cw_pack_map_size(&c, 0);
    }
    if(isActionSucceed){
        cw_pack_unsigned(&c, 0);
    } else {
        cw_pack_unsigned(&c, 1);
    }
    
    pkg->len = c.current - c.start;
    pkg->data = (uint8_t *) malloc(pkg->len);
    memcpy(pkg->data, mpack_buff, pkg->len);
    pkg->len += ESP_CRC_LEN;

    pkg->command = ESP_CMD_PUT_RESULTS;
    if(USE_CRC){
        pkg->flags = 0;
    } else {
        pkg->flags = 1;
    }
}

bool isDeviceBusy(){
    return !ebv_esp_gpio_readReady();
}

bool waitForDevice(){
    uint8_t timeout = 30;
    while(isDeviceBusy() && timeout){
        timeout--;
        ebv_delay(1000);
    }
    return timeout ? true : false;
}

bool isResponseAvailable(){
    return !ebv_esp_gpio_readIRQ();
}

bool waitForResponse(){
    uint8_t timeout = 30;
    while( !isResponseAvailable() && timeout){
        timeout--;
        ebv_delay(1000);
    }
    return timeout ? true : false;
}
