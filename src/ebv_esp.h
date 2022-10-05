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

#ifndef INC_EBV_ESP_H
#define INC_EBV_ESP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ebv_conf.h"

#define DEFAULT_DEVICE_ADDRESS 0x35

#define ESP_CMD_NO_COMMAND 0x00
#define ESP_CMD_READ_DELAYED_RESP 0xA1
#define ESP_CMD_GET_ACTIONS 0xA2
#define ESP_CMD_PUT_RESULTS 0xA3
#define ESP_CMD_PUT_EVENTS 0xA4
#define ESP_CMD_PERFORM_ACTIONS 0xA6
#define ESP_CMD_READ_LOCAL_FILE 0xA7
#define ESP_CMD_READ_LOCAL_FILE 0xA7
#define ESP_CMD_UPDATE_GNSS_LOCATION 0xA8
#define ESP_CMD_PWR_MODE  0xA9

#define ESP_RESPONSE_SOP_SOA_ID 0x56
#define ESP_RESPONSE_SOP_SOR_ID 0x55

#define ESP_DL_PAYLOAD_KIND_ERROR   0xB3A5

// ESP ERROR CODES
typedef enum {
    ESP_ERR_INVALID_CMD_ID = 0x0101,
    ESP_ERR_INVALID_CMD_DATA = 0x0102,
    ESP_ERR_INVALID_RESP_DATA,
    ESP_ERR_INVALID_RESP_CMD_ID,
    ESP_ERR_NETWORK_NOT_AVAILABLE,                      // Network error, mqtt module error
    ESP_ERR_INVALID_CLOUD_RESPONSE,                     // IoT message validation failed
    ESP_ERR_INTERNAL_ERROR,                             // Internal error, not handled exception, bug
    ESP_ERR_RESOURCE_BUSY                               // Introduced for EFTP write operation, means the file cache is full
} esp_err_t;

struct esp_packet_s{
    uint8_t command;
    uint8_t flags;
    uint16_t len;
    uint8_t data[IOT_MSG_MAX_LEN];
    uint32_t crc32;
};

struct esp_response_s{
    uint8_t sop;
    uint8_t command;
    uint16_t len;
    uint8_t response[IOT_MSG_MAX_LEN];
    uint16_t response_len;
    uint8_t *payload;
    uint16_t payload_len;
    bool    has_error_code;
};


enum {
    /* Pin configuration… 
     * All the pins are "active LOW"
     */
    ARDUINO_ESP_MASTER_RESET_SLAVE_PIN = 4,
    ARDUINO_ESP_MASTER_WAKE_SLAVE_PIN = 2,
    ARDUINO_ESP_MASTER_IRQ_PIN = 7,
    ARDUINO_ESP_MASTER_READY_PIN = 8,
    /* …Pin configuration */

    ARDUINO_ESP_MASTER_I2C_MAX_MPACK_SIZE = 128,
};

typedef enum {
    EBV_ESP_RESP_RES_OK,
    EBV_ESP_RESP_RES_ERR,
    EBV_ESP_RESP_RES_INVALID,
} ebv_esp_resp_res_t;


typedef struct esp_packet_s esp_packet_t;
typedef struct esp_response_s esp_response_t;


void ebv_esp_setDeviceAddress(uint8_t addr);
void ebv_esp_packetBuilderByArray(esp_packet_t *pkg, uint8_t command, uint8_t* data, uint16_t data_len);
bool ebv_esp_sendCommand(esp_packet_t *pkg);
bool ebv_esp_submitPacket(esp_packet_t *pkg);
bool ebv_esp_queryDelayedResponse(esp_response_t *resp);
bool ebv_esp_receiveResponse(esp_packet_t *pkg, esp_response_t *resp);
void ebv_esp_dumpPayload(uint8_t *payload, uint8_t payload_len);
uint32_t ebv_esp_getActionId( uint8_t *mpack_action_payload, uint8_t len );
void ebv_esp_buildActionResponse(esp_packet_t *pkg, uint32_t action_id, uint8_t *mpack_response_details, uint8_t response_details_len, bool isActionSucceed);
ebv_esp_resp_res_t ebv_esp_eval_delayed_resp(esp_response_t *resp, uint8_t trigger_esp_cmd);
void ebv_esp_wakeup_device();
bool waitForResponse();
bool waitForDevice();
bool wait_response_available();


#endif