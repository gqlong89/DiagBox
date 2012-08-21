/**********************************************************************
* $Id$		client.h				2012-07-25
*//**
* @file		client.h
* @brief	The Client Parser
* @version	1.0
* @date		25. July. 2012
* @author	Rocky Weshare Xu
*
* Copyright(C) 2012, NCS
* All rights reserved.
*
**********************************************************************/

#ifndef __CLIENT_H__
#define __CLIENT_H__

#define HEADER_1 0x55
#define HEADER_2 0xAA
#define CFG_MODE 0x00
#define OBD_MODE 0x01
#define CFG_GET_VERSION 0x01
#define CFG_K_LINE 0x02
#define CFG_CAN_LINE 0x03
#define CFG_SET_KEEP_LINK_CMD 0xFE
#define CFG_EXIT_OBD 0xFF
#define RECV_HEADER_1 0x01
#define RECV_HEADER_2 0x02
#define RECV_LENGTH_1 0x03
#define RECV_LENGTH_2 0x04
#define RECV_DATA 0x05
#define RECV_CHECKSUM 0x06
#define ERR_NONE 0x00
#define ERR_HEADER_1 0x01
#define ERR_HEADER_2 0x02
#define ERR_CHECKSUM 0x03
#define ERR_TIMEOUT_RECV 0x04
#define ERR_DATA_LENGTH 0x05

typedef void (*CommandParser)(uint8_t *b);
void ClientTaskCreate(void);
void ECUResponse(uint8_t *buf, uint32_t len);

#endif /* __CLIENT_H__ */
