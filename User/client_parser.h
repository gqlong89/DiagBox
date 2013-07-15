/**********************************************************************
* $Id$		client_parser.h			2013-07-09
*//**
* @file		client_parser.h
* @brief	Client Parser Functions
* @version	1.0
* @date		09 Jul. 2013
* @author	Rocky Weshare Xu
*
* Copyright(C) 2013, DNT Diagnostic And Tools
* All rights reserved.
*
**********************************************************************/
#ifndef __CLIENT_PARSER_H__
#define __CLIENT_PARSER_H__

/* Includes ------------------------------------------------------------------- */
#include "core_cm3.h"
#include "ringbuffer.h"
#include "ucos_ii.h"
#include "client.h"

/* Public Type Definition ----------------------------------------------------- */
typedef void (*CommandParser)(uint8_t *b);
typedef void (*CfgParserFunc)(uint8_t *b);
typedef void (*CfgExeFunc)(uint8_t *cmd, uint16_t length);

#define CLIENT_EXIT_OBD_COMMAND_LENGTH 6
#define CLIENT_VERSION_LENGTH 10

/**
 * @brief Configurate Data Struct
 */
typedef struct _CfgDataStruct
{
    int32_t current;
    uint16_t length;
    uint16_t index;
    uint8_t checksum;
    uint8_t data[256];
} CfgDataStruct;

/**
  * @brief Client Parser Fields
  */
typedef struct _ClientParserStruct
{
    CfgDataStruct data; // Config Data Struct data
    RingBuffer buffer; // Message From Client Buffer.
    int32_t mode; // Running mode, CLIENT_CFG_MODE and CLIENT_OBD_MODE
    OS_EVENT *event; // OS_EVENT From Client.
    CommandParser parser[CLIENT_MODE_CNT]; // Command Parser Function table
    CfgParserFunc cfg[CLIENT_RECV_TYPE_CNT]; // Config Parser Function table
    CfgExeFunc exe[CLIENT_CFG_EXE_CNT]; // Config Command Execute Function table
    uint8_t exit[CLIENT_EXIT_OBD_COMMAND_LENGTH];
    uint8_t version[CLIENT_VERSION_LENGTH];
} ClientParserStruct;

/*********************************************************************//**
 * Extern variable
 **********************************************************************/
extern ClientParserStruct client_parser;

/* Public Functions ----------------------------------------------------------- */
/* Client Parser Functions --------------*/
void ClientHardwareInit(void);
void ClientParserStructInit(void);
void ClientParserTask(void *p_arg);

#endif /* __CLIENT_PARSER_H__ */
