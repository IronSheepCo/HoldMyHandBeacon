/* Copyright (c) Nordic Semiconductors ASA
 *All rights reserved.
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

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "debug_service.h"
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_gattc.h"
#include "app_util.h"
#include "SEGGER_RTT.h"

//var for advertising interval
//this way the service can update the main loop
extern int APP_ADV_INTERVAL;

//var for tx power level
//valid values: -40, -30, -20, -16, -12, -8, -4, 0, 4
//this way the service can update the main loop
extern uint8_t APP_TX_LEVEL;

//var for position on x
//this way the service can update the main loop
extern float APP_POS_X;

//var for position on y
//this way the service can update the main loop
extern float APP_POS_Y;

//var for altitute
//this way the service can update the main loop
extern float APP_ALT;

//var for measured tx at 1 m
//this way the service can update the main loop
extern int8_t APP_TX_AT_1M;

void on_connect(ble_os_t* p_service, ble_evt_t* p_ble_evt)
{
    p_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
} 

void on_disconnect(ble_os_t* p_service, ble_evt_t* p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_service->conn_handle = BLE_CONN_HANDLE_INVALID;
}

void on_write(ble_os_t* p_service, ble_evt_t* p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    
    char float_string[32];
    
    memset( float_string, 0, sizeof(float_string) );
    
    if(p_evt_write->handle == p_service->adv_interval_handles.value_handle ){
        SEGGER_RTT_printf(0,"advertising interval written %d\n", *(int*)p_evt_write->data);
        APP_ADV_INTERVAL = *(int*)p_evt_write->data;
    }
    
    if(p_evt_write->handle == p_service->tx_power_handles.value_handle){
        SEGGER_RTT_printf(0,"tx power level written %d\n",*(uint8_t*)p_evt_write->data);
        APP_TX_LEVEL = *(uint8_t*)p_evt_write->data;
        sd_ble_gap_tx_power_set(APP_TX_LEVEL);
    }
    
    if(p_evt_write->handle == p_service->x_handles.value_handle){
        sprintf(float_string, "pos x set to: %d cm\n", *(int*)p_evt_write->data);
        SEGGER_RTT_WriteString(0, float_string);
        memcpy(&APP_POS_X, p_evt_write->data, sizeof(int));
    }
    
    if(p_evt_write->handle == p_service->y_handles.value_handle){
        sprintf(float_string, "pos y set to: %d cm\n", *(int*)p_evt_write->data);
        SEGGER_RTT_WriteString(0, float_string);
        memcpy(&APP_POS_Y, p_evt_write->data, sizeof(float));
    }
    
    if(p_evt_write->handle == p_service->alt_handles.value_handle){
        sprintf(float_string, "altitute set to: %d cm \n", *(int*)p_evt_write->data);
        SEGGER_RTT_WriteString(0, float_string);
        memcpy(&APP_ALT, p_evt_write->data, sizeof(int));
    }
    
    if(p_evt_write->handle == p_service->tx_measured_handles.value_handle){
        SEGGER_RTT_printf(0, "tx measured power set to %d\n", *(int8_t*)p_evt_write->data);
        memcpy(&APP_TX_AT_1M, p_evt_write->data, sizeof(int8_t));
    }
    
    /*if(p_evt_write->handle == p_service->menu_handles.cccd_handle)
    {
       send_next_menu_chunk(p_service, p_ble_evt); 
    }*/
}

/*
 adding a characteristic to the debug service
 */
uint32_t init_characteristic(ble_os_t* p_our_service, ble_gatts_char_handles_t* handles, char* name, int uuid, void* p_value_location, int8_t max_len )
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    
    //
    memset(&cccd_md,0,sizeof(cccd_md));
    
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    
    //setting the char metadata
    memset(&char_md, 0, sizeof(ble_gatts_char_md_t) );
    
    char_md.char_props.write = 1;
    char_md.char_props.read = 1;
    char_md.char_props.notify = 0;
    //char_md.p_cccd_md = &cccd_md;
    //char_md.p_sccd_md = NULL;
    char_md.p_char_user_desc = (uint8_t*)name;
    char_md.char_user_desc_max_size = strlen(name);
    char_md.char_user_desc_size = char_md.char_user_desc_max_size;
    
    ble_uuid.type = p_our_service->uuid_type;
    ble_uuid.uuid = uuid;
    
    //attributes metadata
    memset(&attr_md, 0, sizeof(ble_gatts_attr_md_t));
    
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc    =   BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth =   0;
    attr_md.vlen    =   0;
    
    //characteristic info
    memset(&attr_char_value, 0, sizeof(ble_gatts_attr_t));
    
    attr_char_value.p_uuid      =   &ble_uuid;
    attr_char_value.p_attr_md   =   &attr_md;
    attr_char_value.init_len    =   max_len;
    attr_char_value.init_offs   =   0;
    attr_char_value.max_len     =   max_len;
    attr_char_value.p_value     =   p_value_location;
    
    return sd_ble_gatts_characteristic_add(p_our_service->service_handle,
                                           &char_md, &attr_char_value,
                                           handles);
}

uint32_t init_gatt(ble_os_t* p_our_service)
{
    init_characteristic(p_our_service, &p_our_service->tx_measured_handles, "MeasuredTx", 0xA33A, (void*)&APP_TX_AT_1M, sizeof(int8_t));
    
    init_characteristic(p_our_service, &p_our_service->adv_interval_handles, "AdvInteval", 0xA335, (void*)&APP_ADV_INTERVAL, sizeof(int));
    
    init_characteristic(p_our_service, &p_our_service->tx_power_handles, "TxPower", 0xA336, (void*)&APP_TX_LEVEL, sizeof(uint8_t));
    
    init_characteristic(p_our_service, &p_our_service->x_handles, "Pos x", 0xA337, (void*)&APP_POS_X, sizeof(int) );
    
    init_characteristic(p_our_service, &p_our_service->y_handles, "Pos y", 0xA338, (void*)&APP_POS_Y, sizeof(int) );
    
    return init_characteristic(p_our_service, &p_our_service->alt_handles, "Altitude", 0xA339, (void*)&APP_ALT, sizeof(int) );
}

/**@brief Function for initiating our new service.
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */
void debug_service_init(ble_os_t * p_our_service)
{
    uint32_t   err_code; // Variable to hold return codes from library and softdevice functions

    p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
    
    // OUR_JOB: Declare 16 bit service and 128 bit base UUIDs and add them to BLE stack table     
    ble_uuid_t  service_uuid;
    ble_uuid128_t   base_uuid = {BLE_UUID_DEBUG_BASE_UUID};

    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_our_service->uuid_type);
    
    APP_ERROR_CHECK(err_code);   
 
    service_uuid.type = p_our_service->uuid_type;
    service_uuid.uuid = BLE_UUID_DEBUG_SERVICE;

    // OUR_JOB: Add our service
    err_code = sd_ble_gatts_service_add( BLE_GATTS_SRVC_TYPE_PRIMARY,
                                         &service_uuid,
                                         &p_our_service->service_handle);
    APP_ERROR_CHECK(err_code);

    err_code = init_gatt(p_our_service);
    APP_ERROR_CHECK(err_code);
}

void debug_service_on_ble_evt(ble_os_t* p_service, ble_evt_t* p_ble_evt)
{
    switch( p_ble_evt->header.evt_id )
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_service, p_ble_evt);
        break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_service, p_ble_evt);
        break;

//        case :
//            send_next_menu_chunk(p_service, p_ble_evt);            
//        break;       
        case BLE_GATTS_EVT_WRITE:
            SEGGER_RTT_WriteString(0, "write event\n");
            on_write(p_service, p_ble_evt);
        break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            SEGGER_RTT_WriteString(0, "missing sys attr\n");
            sd_ble_gatts_sys_attr_set(p_ble_evt->evt.gatts_evt.conn_handle, NULL,0,BLE_GATTS_SYS_ATTR_FLAG_USR_SRVCS);
        break;
 
        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            {
            ble_gatts_rw_authorize_reply_params_t reply;
            memset(&reply,0,sizeof(reply));
            
            reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;

            sd_ble_gatts_rw_authorize_reply(p_service->conn_handle, &reply);
            }
        break;

        case BLE_EVT_TX_COMPLETE:
        break;

        default:
        break;
    }
}
