/**********************************************************************
* $Id$		client.h			2013-07-09
*//**
* @file		client.h
* @brief	DiagBox Client Commbox macros, functions define
* @version	1.0
* @date		09 Jul. 2013
* @author	Rocky Weshare Xu
*
* Copyright(C) 2013, DNT Diagnostic And Tools
* All rights reserved.
*
**********************************************************************/


#ifndef __CLIENT_H__
#define __CLIENT_H__

/* Includes ------------------------------------------------------------------- */
#include <stdlib.h>
#include <stdint.h>
#include <cpu.h>

/*********************************************************************//**
 * Macro defines for Client Message Format
 **********************************************************************/
/** All message must start with 0x55 0xAA */ 
#define CLIENT_HEADER_1 0x55
#define CLIENT_HEADER_2 0xAA
/** Client command mode */ 
#define CLIENT_CFG_MODE 0x00
#define CLIENT_OBD_MODE 0x01
#define CLIENT_MODE_CNT 0x02
/** Client configuration options */ 
#define CLIENT_CFG_NONE 0x00
#define CLIENT_CFG_GET_VERSION 0x01
#define CLIENT_CFG_OBD_MODE 0x02
#define CLIENT_CFG_SET_HEART_BEAT 0x03
#define CLIENT_CFG_EXIT_OBD 0x04
#define CLIENT_CFG_EXE_CNT 0x05
/** Client receive message status */
#define CLIENT_RECV_HEADER_1 0x00
#define CLIENT_RECV_HEADER_2 0x01
#define CLIENT_RECV_LENGTH_1 0x02
#define CLIENT_RECV_LENGTH_2 0x03
#define CLIENT_RECV_DATA 0x04
#define CLIENT_RECV_CHECKSUM 0x05
#define CLIENT_RECV_TYPE_CNT 0x06
/** Error Code */
#define CLIENT_ERR_NONE 0x00
#define CLIENT_ERR_HEADER_1 0x01
#define CLIENT_ERR_HEADER_2 0x02
#define CLIENT_ERR_CHECKSUM 0x03
#define CLIENT_ERR_TIMEOUT_RECV 0x04
#define CLIENT_ERR_DATA_LENGTH 0x05
#define CLIENT_ERR_OBD_SETTING_FAIL 0x06
#define CLIENT_ERR_OBD_UNSUPPORTED 0x07
#define CLIENT_ERR_OBD_START_FAIL 0x08

/* Public Functions ----------------------------------------------------------- */
/* Client functions -------------------*/
CPU_INT08U ClientParserTaskCreate(void);
CPU_INT08U ClientResponseTaskCreate(void);
void ECUResponse(uint8_t *buf, uint32_t len);

#endif /* __CLIENT_H__ */
