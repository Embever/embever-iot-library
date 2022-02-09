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

#ifndef INC_EBV_ESP_GPIO_H
#define INC_EBV_ESP_GPIO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef bool    (*_ebv_esp_gpio_readReady)();
typedef bool    (*_ebv_esp_gpio_readIRQ)();

struct ebv_esp_gpio_cb{
    _ebv_esp_gpio_readReady     readReady;
    _ebv_esp_gpio_readIRQ       readIRQ;
};

void    ebv_esp_gpio_registerGpio(struct ebv_esp_gpio_cb *cb);
bool    ebv_esp_gpio_readReady();
bool    ebv_esp_gpio_readIRQ();

#if defined(ESP32)
    #define DEFAULT_PIN_EBV_READY   23
    #define DEFAULT_PIN_EBV_IRQ     19
#elif defined (__AVR__)
    #define ARDUINO_NANO_PIN_A2  16
    #define ARDUINO_NANO_PIN_A3  17
    #define DEFAULT_PIN_EBV_READY   ARDUINO_NANO_PIN_A3
    #define DEFAULT_PIN_EBV_IRQ     ARDUINO_NANO_PIN_A2
#endif

#ifndef PIN_EBV_READY
#define PIN_EBV_READY DEFAULT_PIN_EBV_READY
#endif

#ifndef PIN_EBV_IRQ
#define PIN_EBV_IRQ DEFAULT_PIN_EBV_IRQ
#endif

#define EBV_ESP_SETUP_ARDUINO_GPIO_CB                    \
    bool gpio_readReady(){                               \
        return digitalRead(PIN_EBV_READY);               \
    }                                                    \
    bool gpio_readIrq(){                                 \
        return digitalRead(PIN_EBV_IRQ);                 \
    }

#define EBV_ESP_REGISTER_ARDUINO_GPIO_CB                 \
    pinMode(PIN_EBV_READY, INPUT);                       \
    pinMode(PIN_EBV_IRQ, INPUT);                         \
    struct ebv_esp_gpio_cb gpio_cb;                      \
    gpio_cb.readReady = gpio_readReady;                  \
    gpio_cb.readIRQ = gpio_readIrq;                      \
    ebv_esp_gpio_registerGpio(&gpio_cb);

#endif