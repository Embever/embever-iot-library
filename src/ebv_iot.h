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
#include "ebv_delay.h"
#include "ebv_i2c.h"
#include "ebv_esp_gpio.h"

#define EBV_SETUP_ARDUINO_CB    EBV_SETUP_ARDUINO_WIRE_CB EBV_ESP_SETUP_ARDUINO_GPIO_CB;
#define EBV_REGISTER_ARDUINO_CB EBV_I2C_REGISTER_ARDUINO_WIRE; EBV_ESP_REGISTER_ARDUINO_GPIO_CB; EBV_DELAY_REGISTER_ARDUINO;

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

typedef struct{
    uint8_t *body;
    uint8_t len;
    bool result;
} ebv_iot_event;

bool _ebv_iot_addUnsignedPayload(char * k, unsigned int v);
bool _ebv_iot_addSignedPayload(char * k, int v);
bool _ebv_iot_addFloatPayload(char * k, float v);
bool _ebv_iot_addDoublePayload(char * k, double v);
bool _ebv_iot_addStringPayload(char * k, char * v);
bool _ebv_iot_addCharPayload(char * k, char v);

// Overloaded payload packer
#ifdef __cplusplus
    void ebv_iot_addGenericPayload(char * key, unsigned int value);
    void ebv_iot_addGenericPayload(char * key, int value);
    void ebv_iot_addGenericPayload(char * key, float value);
    void ebv_iot_addGenericPayload(char * key, double value);
    void ebv_iot_addGenericPayload(char * key, char * value);
    void ebv_iot_addGenericPayload(char * key, char value);
#else
    #define ebv_iot_addGenericPayload(key,value) _Generic( value,                                  \
                                                    unsigned int:   _ebv_iot_addUnsignedPayload,      \
                                                    int:            _ebv_iot_addSignedPayload,      \
                                                    float :         _ebv_iot_addFloatPayload,            \
                                                    double :        _ebv_iot_addDoublePayload,           \
                                                    char *:         _ebv_iot_addStringPayload,               \
                                                    char :          _ebv_iot_addCharPayload               \
                                                    )(key,value)
#endif
void ebv_iot_receiveAction(esp_response_t *response);
bool ebv_iot_parseAction(esp_response_t *resp, ebv_action_t *action );
bool ebv_iot_submitActionResult(ebv_action_t *a, esp_response_t *response);
bool ebv_iot_submitGenericActionResult(ebv_action_t *a, esp_response_t *response);
bool ebv_iot_submitEvent(ebv_iot_event *e);
bool ebv_iot_submitGenericEvent();
bool ebv_iot_initGenericEvent(char * evnt_type);
bool ebv_iot_initGenericResponse();

#endif