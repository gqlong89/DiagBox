/**********************************************************************
* $Id$		obd_exe.h			2013-07-09
*//**
* @file		obd_exe.h
* @brief	OBD command from client execute
* @version	1.0
* @date		09 Jul. 2013
* @author	Rocky Weshare Xu
*
* Copyright(C) 2013, DNT Diagnostic And Tools
* All rights reserved.
*
**********************************************************************/
#ifndef __OBD_PARSER_H__
#define __OBD_PARSER_H__

/* Includes ------------------------------------------------------------------- */
#include <stdint.h>
#include <stdlib.h>

/*********************************************************************//**
 * Macro defines for OBD Execute
 **********************************************************************/
/** OBD ECU Now we supported */
#define OBD_MIKUNI 0x00
#define OBD_DENSO 0x01
#define OBD_SYNERJECT 0x02
#define OBD_VISTEON 0x03
#define OBD_BOSCH 0x04
#define OBD_SUPPORT_ECU_CNT 0x05
#define OBD_HEART_BEAT_LENGTH 16

/* Public Types --------------------------------------------------------------- */
typedef uint8_t (*CfgExeEcuInitFunc)(uint8_t *cmd, uint16_t length);
typedef void (*OBDSendCommandFunc)(uint8_t *cmd, size_t length);
typedef struct _ObdExeStruct
{
    uint8_t heart_beat[OBD_HEART_BEAT_LENGTH];
    CfgExeEcuInitFunc exe[OBD_SUPPORT_ECU_CNT];
    OBDSendCommandFunc send;
} ObdExeStruct;

/* Public Variables ----------------------------------------------------------- */
extern ObdExeStruct client_obd;
/* Public Functions ----------------------------------------------------------- */
void CfgExeOBDInit(void);
void CfgExeOBDMode(uint8_t *cmd, uint16_t length);

/* Public Variable ----------------------------------------------------------- */
extern uint8_t heart_beat_buffer[16];
extern OBDSendCommandFunc OBDSend;

#endif /* __OBD_PARSER_H__ */

