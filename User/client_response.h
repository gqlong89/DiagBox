/**********************************************************************
* $Id$		client_response.h			2013-07-09
*//**
* @file		client_response.h
* @brief	Client Response Functions
* @version	1.0
* @date		09 Jul. 2013
* @author	Rocky Weshare Xu
*
* Copyright(C) 2013, DNT Diagnostic And Tools
* All rights reserved.
*
**********************************************************************/
#ifndef __CLIENT_RESPONSE_H__
#define __CLIENT_RESPONSE_H__

/* Includes ------------------------------------------------------------------- */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "core_cm3.h"
#include "ringbuffer.h"
#include "ucos_ii.h"

/* Public Type Definition ----------------------------------------------------- */
/**
 * @brief Client Response Struct
 */
typedef struct _ClientResponseStruct
{
    RingBuffer buffer;
    OS_EVENT *event;
    bool is_response;
} ClientResponseStruct;
/* Public Variables ----------------------------------------------------------- */
extern ClientResponseStruct client_res;
/* Public Functions ----------------------------------------------------------- */
void ClientResponseInit(void);
/* Client Response Functions ----------------*/
void SendToClientPack(uint8_t *buff, size_t len);
void SendToClientBytePack(uint8_t byte);
void SendToClientNoPack(uint8_t *buff, size_t len);
void SendToClientByteNoPack(uint8_t byte);
void ClientResponseTask(void *p_arg);
void ClientResponseRun(bool is_run);

#endif /* __CLIENT_RESPONSE_H__ */
