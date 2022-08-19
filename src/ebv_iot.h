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

#ifndef INC_EBV_IOT_H
#define INC_EBV_IOT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ebv_esp.h"
#include "ebv_local.h"
#include "ebv_delay.h"
#include "ebv_i2c.h"
#include "ebv_esp_gpio.h"
#include "ebv_conf.h"
#include "extcwpack.h"
#include "utils/va_arg_helper.h"
#include "utils/ebv_utils.h"

#define EBV_SETUP_ARDUINO_CB    EBV_SETUP_ARDUINO_WIRE_CB EBV_ESP_SETUP_ARDUINO_GPIO_CB;
#define EBV_REGISTER_ARDUINO_CB EBV_I2C_REGISTER_ARDUINO_WIRE; EBV_ESP_REGISTER_ARDUINO_GPIO_CB; EBV_DELAY_REGISTER_ARDUINO;
// Build an mpack list out of unsigned integers, can take various number of args
#define EBV_IOT_BUILD_STATIC_UINT_LIST(list_name, ...)                                              \
            uint8_t list_name##_static_mpack_buf[(IOT_MSG_MAX_LEN / 2) + (IOT_MSG_MAX_LEN / 4)];      \
            ebv_iot_custom_msg_data_t list_name;                                                    \
            list_name.buf = list_name##_static_mpack_buf;                                             \
            list_name.buf_len = (IOT_MSG_MAX_LEN / 2) + (IOT_MSG_MAX_LEN / 4);                       \
            list_name._type=EBV_IOT_CUSTOM_MSG_TYPE_LIST;                                           \
            _ebv_iot_build_uint_list(&list_name, PP_NARG(__VA_ARGS__), __VA_ARGS__)

#define EBV_IOT_BUILD_STATIC_UINT_LIST_BY_ARRAY(list_name, array, nof_elements)                     \
            uint8_t list_name##_static_mpack_buf[(IOT_MSG_MAX_LEN / 2) + (IOT_MSG_MAX_LEN / 4)];      \
            ebv_iot_custom_msg_data_t list_name;                                                    \
            list_name.buf = list_name##_static_mpack_buf;                                             \
            list_name.buf_len = (IOT_MSG_MAX_LEN / 2) + (IOT_MSG_MAX_LEN / 4);                       \
            list_name._type=EBV_IOT_CUSTOM_MSG_TYPE_LIST;                                           \
            ebv_iot_build_uint_list_by_array(&list_name, array, nof_elements)

#define EBV_IOT_BUILD_STATIC_INT_LIST_BY_ARRAY(list_name, array, nof_elements)                      \
            uint8_t list_name##_static_mpack_buf[(IOT_MSG_MAX_LEN / 2) + (IOT_MSG_MAX_LEN / 4)];    \
            ebv_iot_custom_msg_data_t list_name;                                                    \
            list_name.buf = list_name##_static_mpack_buf;                                           \
            list_name.buf_len = (IOT_MSG_MAX_LEN / 2) + (IOT_MSG_MAX_LEN / 4);                       \
            ebv_iot_custom_msg_data_t list_name;                                                    \
            list_name._type=EBV_IOT_CUSTOM_MSG_TYPE_LIST;                                           \
            ebv_iot_build_int_list_by_array(&list_name, array, nof_elements)

#define EBV_IOT_BUILD_INT_LIST_BY_ARRAY(list_name, array, nof_elements)                             \
            list_name._type=EBV_IOT_CUSTOM_MSG_TYPE_LIST;                                           \
            ebv_iot_build_int_list_by_array(&list_name, array, nof_elements)



typedef struct{
    uint32_t id;
    char type[16];
    char payload[32];
    uint8_t payload_len;
    uint8_t svr_lvl;
    bool result;
    uint8_t *response_payload;
    uint8_t response_payload_size;
} ebv_action_t;

typedef enum {
    EBV_ESP_COM_ERROR_NONE,
    EBV_ESP_COM_ERROR_ACK_TIMEOUT,
    EBV_ESP_COM_ERROR_RESPONSE_TIMEOUT,
    EBV_ESP_COM_ERROR_ACK_INVALID,
    EBV_ESP_COM_ERROR_RESPONSE_INVALID,
    EBV_ESP_COM_ERROR_BUSY_TIMEOUT
} ebv_esp_com_error_t;

typedef struct{
    uint8_t *body;
    uint8_t len;
    bool result;
} ebv_iot_event;

enum ebv_iot_custom_msg_type{
    EBV_IOT_CUSTOM_MSG_TYPE_LIST = 0
};

typedef struct{
    //uint8_t buf[(IOT_MSG_MAX_LEN / 2) + (IOT_MSG_MAX_LEN / 4)];
    uint8_t *buf;
    uint8_t buf_len;
    enum ebv_iot_custom_msg_type _type;
} ebv_iot_custom_msg_data_t;

typedef struct{
    uint8_t buff[IOT_MSG_MAX_LEN];          // This is the mpack buffer
    uint16_t size;                           // The current size of the buffer
    uint8_t elements;                       // Count of the elements in the mpacked content
    bool isBufferReady;                     // Flag for indicating the budder status
    cw_pack_context c;                      // mpack struct for keep track about the packing
} ebv_mpack;

bool _ebv_iot_addUnsignedPayload(const char * k, unsigned int v);
bool _ebv_iot_addSignedPayload(const char * k, int v);
bool _ebv_iot_addFloatPayload(const char * k, float v);
bool _ebv_iot_addDoublePayload(const char * k, double v);
bool _ebv_iot_addStringPayload(const char * k, char * v);
bool _ebv_iot_addCharPayload(const char * k, char v);

// Overloaded payload packer
#ifdef __cplusplus
    void ebv_iot_addGenericPayload(const char * key, unsigned int value);
    void ebv_iot_addGenericPayload(const char * key, int value);
    void ebv_iot_addGenericPayload(const char * key, float value);
    void ebv_iot_addGenericPayload(const char * key, double value);
    void ebv_iot_addGenericPayload(const char * key, const char * value);
    void ebv_iot_addGenericPayload(const char * key, const char value);
    void ebv_iot_addGenericPayload(const char * key, ebv_iot_custom_msg_data_t  *value);
    void ebv_iot_build_uint_list_by_array(ebv_iot_custom_msg_data_t *l, const uint8_t *array, uint8_t nof_elements);
    void ebv_iot_build_uint_list_by_array(ebv_iot_custom_msg_data_t *l, const uint16_t *array, uint8_t nof_elements);
    void ebv_iot_build_int_list_by_array(ebv_iot_custom_msg_data_t *l, const int16_t *array, uint8_t nof_elements);
#else
    #define ebv_iot_addGenericPayload(key,value) _Generic( value,                                           \
                                                    unsigned int:   _ebv_iot_addUnsignedPayload,            \
                                                    int:            _ebv_iot_addSignedPayload,              \
                                                    float :         _ebv_iot_addFloatPayload,               \
                                                    double :        _ebv_iot_addDoublePayload,              \
                                                    char *:         _ebv_iot_addStringPayload,              \
                                                    char :          _ebv_iot_addCharPayload                 \
                                                    default :       _ebv_iot_addCustomPayload               \
                                                    )(key,value)
    #define ebv_iot_build_uint_list_by_array(l, buff, len) _Generic(buff,                                  \
                                                    uint8_t *: _ebv_iot_build_uint8_list_by_array,          \
                                                    uint16_t *: _ebv_iot_build_uint16_list_by_array,        \
                                                    )(l, buff, len)
#endif
void ebv_iot_init();
ebv_ret_t ebv_iot_receiveAction(esp_response_t *response);
bool ebv_iot_parseAction(esp_response_t *resp, ebv_action_t *action );
ebv_ret_t ebv_iot_submitActionResult(ebv_action_t *a, esp_response_t *response);
ebv_ret_t ebv_iot_submitGenericActionResult(ebv_action_t *a, esp_response_t *response);
bool ebv_iot_submitEvent(ebv_iot_event *e);
bool ebv_iot_submitGenericEvent();
bool ebv_iot_initGenericEvent(const char * evnt_type);
bool ebv_iot_initGenericResponse();
ebv_esp_com_error_t ebv_iot_get_last_error_code();
void ebv_iot_dump_last_error();
void _ebv_iot_build_uint_list(ebv_iot_custom_msg_data_t *l, uint8_t num_args, ...);

#endif