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

#ifndef INC_EBV_I2C_H
#define INC_EBV_I2C_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void    (*_ebv_i2c_beginTransaction)(uint8_t device_address);
typedef void    (*_ebv_i2c_stopTransaction)();
typedef int     (*_ebv_i2c_available)();
typedef size_t  (*_ebv_i2c_write)(uint8_t data);
typedef size_t    (*_ebv_i2c_request)(uint8_t device_address, uint16_t nof_bytes);
typedef int     (*_ebv_i2c_read)();

struct ebv_i2c_cb{
    _ebv_i2c_beginTransaction    beginXfer;
    _ebv_i2c_stopTransaction     stopXfer;
    _ebv_i2c_available           dataAvailable;
    _ebv_i2c_write               dataWrite;
    _ebv_i2c_request             dataRequest;
    _ebv_i2c_read                dataRead;
};

void    ebv_i2c_registerI2c(struct ebv_i2c_cb *cb);
void    ebv_i2c_I2cBeginTransaction(uint8_t device_address);
void    ebv_i2c_I2cFinishTransaction();
int     ebv_i2c_I2cAvailable();
size_t  ebv_i2c_I2cWrite(uint8_t data);
size_t  ebv_i2c_I2cRequest(uint8_t address, uint16_t nof_bytes);
int     ebv_i2c_I2cRead();

#define EBV_SETUP_ARDUINO_WIRE_CB                                   \
    void wire_begin(uint8_t address){                               \
        Wire.beginTransmission(address);                            \
    }                                                               \
    void wire_end(){                                                \
        Wire.endTransmission();                                     \
    }                                                               \
    int wire_available(){                                           \
        return Wire.available();                                    \
    }                                                               \
    size_t wire_write(uint8_t data){                                \
        return Wire.write(data);                                    \
    }                                                               \
    size_t wire_requestFrom(uint8_t address, uint16_t nof_bytes){   \
       return Wire.requestFrom((int) address, (int) nof_bytes);     \
    }                                                               \
    int wire_read(){                                                \
        return Wire.read();                                         \
    }                                                           

#define EBV_I2C_REGISTER_ARDUINO_WIRE                           \
    struct ebv_i2c_cb cb;                                       \
    cb.beginXfer = wire_begin;                                  \
    cb.stopXfer = wire_end;                                     \
    cb.dataAvailable = wire_available;                          \
    cb.dataWrite = wire_write;                                  \
    cb.dataRequest = wire_requestFrom;                           \
    cb.dataRead = wire_read;                                    \
    ebv_i2c_registerI2c(&cb);                                   \
    Wire.begin();

#endif