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

bool ebv_i2c_isLayerInitialised = false;
struct ebv_i2c_cb _ebv_i2c_cb;

void ebv_i2c_registerI2c(struct ebv_i2c_cb *cb){
    _ebv_i2c_cb.beginXfer       = cb->beginXfer;
    _ebv_i2c_cb.stopXfer        = cb->stopXfer;
    _ebv_i2c_cb.dataAvailable   = cb->dataAvailable;
    _ebv_i2c_cb.dataWrite       = cb->dataWrite;
    _ebv_i2c_cb.dataReqest      = cb->dataReqest;
    _ebv_i2c_cb.dataRead        = cb->dataRead;
    ebv_i2c_isLayerInitialised = true;
}

void ebv_i2c_I2cBeginTransaction(uint8_t device_address){
    _ebv_i2c_cb.beginXfer(device_address);
}
void ebv_i2c_I2cFinishTransaction(){
    _ebv_i2c_cb.stopXfer();
}
int ebv_i2c_I2cAvailable(){
    return _ebv_i2c_cb.dataAvailable();
}
size_t ebv_i2c_I2cWrite(uint8_t data){
    return _ebv_i2c_cb.dataWrite(data);
}

size_t ebv_i2c_I2cRequest(uint8_t address, uint8_t nof_bytes){
    return _ebv_i2c_cb.dataReqest(address, nof_bytes);
}

int ebv_i2c_I2cRead(){
    return _ebv_i2c_cb.dataRead();
}

