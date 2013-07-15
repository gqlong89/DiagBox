/**********************************************************************
* $Id$		mikuni.c				2013-07-08
*//**
* @file		mikuni.c
* @brief	Mikuni Source Codes
*				  
* @version	1.0
* @date		08. July. 2013
* @author	Rocky Weshare Xu
*
* Copyright(C) 2013, DNT Diagnostic And Tools
* All rights reserved.
*
**********************************************************************/

/* Includes ------------------------------------------------------------------- */
#include "includes.h"

/*********************************************************************//**
 * @brief 		Mikuni PCM Module initialize
 * @param[in] 	data:   options data
 * @param[in]	len:    options data length
 * @return 		Client Error Code
 ***********************************************************************/
uint8_t MikuniPCMInit(uint8_t *data, uint16_t length)
{
    if (length != 1) return CLIENT_ERR_OBD_SETTING_FAIL;

    k_config.baudrate = 19200;
    k_config.databits = 8;
    k_config.flow_control = K_FLOWCONTROL_NONE;
    k_config.stopbits = K_STOPBITS_ONE;
    client_obd.send = KSendData;

    KBeginSettings();
    switch (data[0]) {
        case MIKUNI_PARITY_NONE: k_config.parity = K_PARITY_NONE; break;
        case MIKUNI_PARITY_ODD: k_config.parity = K_PARITY_ODD; break;
        case MIKUNI_PARITY_EVEN: k_config.parity = K_PARITY_EVEN; break;
        default: 
            KEndSettings();
            return CLIENT_ERR_OBD_SETTING_FAIL;
    }
    
    KEndSettings();    
    return CLIENT_ERR_NONE;
}

/*********************************************************************//**
 * @brief 		Mikuni Initialize for prepare communication
 * @param[in] 	data:   options data
 * @param[in]	length: options data length
 * @return 		Client Error Code
 ***********************************************************************/
uint8_t MikuniInit(uint8_t *data, uint16_t length)
{
    if (length <= 1) return CLIENT_ERR_OBD_SETTING_FAIL;
    
    switch(data[0]) {
        case MIKUNI_PCM: return MikuniPCMInit(data + 1, length - 1);
        default: break;
    }
    
    return CLIENT_ERR_OBD_SETTING_FAIL;
}
