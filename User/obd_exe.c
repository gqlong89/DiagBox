/**********************************************************************
* $Id$		obd_exe.c				2013-07-08
*//**
* @file		obd_exe.c
* @brief	Deal With Client Command Source Code(OBD Command Parser part)
*         Command format
*         HEADER_1 HEADER_2 LENGTH_1 LENGTH_2 Data.... Checksum(XOR)
*         Our Adapter work in modes
*         1. Configuration Mode
*            When Adapter work under this mode, it only accept config command.
*         2. Diagnostic Mode
*            When Adapter work under this mode, it only accept Start Diagnostic/Stop Diagnostic 
*             command and ECU command.
*         Steps:
*           1. Configure parameters..(K line or Can line etc.)
*           2. Start Diagnostic Mode with line initialize(K Line fast init, 5 baud init etc.)
*           3. Sending ECU commands.
*           4. Exit Diagnostic Mode and return to Configuration Mode.
*         Notice:
*           1. When Adapter running in Diagnostic Mode in 1 Min without any message from Client or ECU,
*               adatper will ruturn to Configuration Mode.
*         Commands: (Hex)
*           Work on Configuration Mode:
*             1.  Get Adapter Version : 55 AA 00 01 01 FF
*                 Response            : 55 AA 00 0A 'D' 'A' '-' 'V' '0' '.' '0' '.' '1' '\0' B0
*		      2.  Exit OBD            : 55 AA 00 01 FF 01
*                 Response     		  : 55 AA 00 01 00 01
*             3.  Set Keep Link       : 55 AA xx xx FE xx xx xx xx.. CS
*                 Response            : 55 AA 00 01 00 01
*             4.  Set OBD Mode        : 55 AA xx xx 02 ECU opts..... CS
*                 Response            : Error Code
*				  
* @version	1.0
* @date		08. July. 2013
* @author	Rocky Weshare Xu
*
* Copyright(C) 2013, DNT
* All rights reserved.
*
**********************************************************************/

/* Includes ------------------------------------------------------------------- */
#include "includes.h"
/* Public Variables ----------------------------------------------------------- */
ObdExeStruct client_obd;

/*********************************************************************//**
 * @brief 		OBD Config Execute Initialize
 * @param[in] 	None
 * @return 		None
 ***********************************************************************/
void CfgExeOBDInit(void)
{
    client_obd.exe[OBD_MIKUNI] = MikuniInit;
}

/*********************************************************************//**
 * @brief 		OBD Config Execute Function
 * @param[in] 	cmd:    command buff
 * @param[in]	length: command buff length
 * @return 		None
 ***********************************************************************/
void CfgExeOBDMode(uint8_t *cmd, uint16_t length)
{
    uint8_t err = CLIENT_ERR_DATA_LENGTH;
    if (length > 0) {
        if (cmd[0] >= OBD_SUPPORT_ECU_CNT) {
            err = CLIENT_ERR_OBD_UNSUPPORTED;
        } else {
            err = client_obd.exe[cmd[0]](cmd + 1, length - 1);
        }
    }
    if (err == CLIENT_ERR_NONE) client_parser.mode = CLIENT_OBD_MODE;
    SendToClientBytePack(err);
}
