/* Copyright (c) Nordic Semiconductor ASA
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 *   1. Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 *   2. Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 *   3. Neither the name of Nordic Semiconductor ASA nor the names of other
 *   contributors to this software may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 * 
 *   4. This software must only be used in a processor manufactured by Nordic
 *   Semiconductor ASA, or in a processor manufactured by a third party that
 *   is used in combination with a processor manufactured by Nordic Semiconductor.
 * 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef OUR_SERVICE_H__
#define OUR_SERVICE_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"

//D2D637B6-0B65-44DE-9394-B5AC
//A3
//34
//FF
//D7

#define BLE_UUID_DEBUG_BASE_UUID              {0xD2, 0xD6, 0x37, 0xB6, 0x0B, 0x65, 0x44, 0xDE, 0x93, 0x94, 0xB5, 0xAC, 0x00, 0x00, 0xFF, 0xD7} // 128-bit base UUID
#define BLE_UUID_DEBUG_SERVICE                0xA334


/**
 * @brief This structure contains various status information for our service. 
 * It only holds one entry now, but will be populated with more items as we go.
 * The name is based on the naming convention used in Nordic's SDKs. 
 * 'ble� indicates that it is a Bluetooth Low Energy relevant structure and 
 * �os� is short for Our Service). 
 */
typedef struct
{
    uint16_t    service_handle;     /**< Handle of Our Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t tx_measured_handles;
    ble_gatts_char_handles_t adv_interval_handles;
    ble_gatts_char_handles_t tx_power_handles;
    ble_gatts_char_handles_t x_handles;
    ble_gatts_char_handles_t y_handles;
    ble_gatts_char_handles_t alt_handles;
    uint8_t    uuid_type;
    uint16_t   conn_handle;
}ble_os_t;

/**@brief Function for initializing our new service.
 *
 * @param[in]   p_our_service       Pointer to Our Service structure.
 */
void debug_service_init(ble_os_t * p_our_service);

void debug_service_on_ble_evt(ble_os_t* p_service, ble_evt_t* p_ble_evt);
#endif  /* _ OUR_SERVICE_H__ */
