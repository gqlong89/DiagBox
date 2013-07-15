/**********************************************************************
* $Id$		kline.h			2013-07-09
*//**
* @file		kline.h
* @brief	K Line ECU functions
* @version	1.0
* @date		09 Jul. 2013
* @author	Rocky Weshare Xu
*
* Copyright(C) 2013, DNT Diagnostic And Tools
* All rights reserved.
*
**********************************************************************/
#ifndef __K_LINE_H__
#define __K_LINE_H__

/* Includes ------------------------------------------------------------------- */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "ucos_ii.h"

/* Public Types --------------------------------------------------------------- */
/*********************************************************************//**
 * @brief KLine enumeration
 **********************************************************************/
/** @brief Parity */
typedef enum _KParity
{
	K_PARITY_NONE = 0,
	K_PARITY_ODD = 1,
	K_PARITY_EVEN = 2
} KParity;

/** @brief Floc Control */
typedef enum _KFlowControl
{
	K_FLOWCONTROL_NONE = 0,
	K_FLOWCONTROL_HARDWARE = 1,
	K_FLOWCONTROL_SOFTWARE = 2
} KFlowControl;

/** @brief Stopbits */
typedef enum _KStopbits
{
	K_STOPBITS_ONE = 0,
	K_STOPBITS_TWO = 1
} KStopbits;

/** @brief K Line line initialize type */
typedef enum _KStartType
{
    K_START_NONE = 0,
    K_5BAUD_SYNC = 1,
    K_FAST_INIT = 2
} KStartType;

/** @brief K Line Config Struct */
typedef struct _KConfig
{
    uint32_t low_time; // only for fast init mode, low signal time duration, ms.
    uint32_t high_time; // only for fast init mode, high signal time duratio, ms.
    uint32_t bit_time; // only for 5 baud init, receive 0x55 time.
    uint32_t bit_count; // only for 5 baud init, receive 0x55 bit count;
    uint32_t baudrate;
    KParity parity;
    KFlowControl flow_control;
    KStopbits stopbits;
    OS_EVENT *baudrate_event;
    uint8_t databits;
    uint8_t l_line; // 0 for no l line config.
    uint8_t k_line; // 0 for no meaning.
    uint8_t addr_code; // only for 5 baud init.
    uint8_t keybyte_2; // 0 for not send back !keybyte2, other send back !keybyte2. 
                       // (keybyte2 is ECU response 2nd keybyte).
    uint8_t enter_cmd[16]; // only for fast init mode, after low/high signal, command to send.
} KConfig;

/* Public Variables ----------------------------------------------------------- */
extern KConfig k_config;
/* Public Functions ----------------------------------------------------------- */
/* KLine Hardware Initialize ----------------*/
void KHardwareInit(void);
void KConfigInit(void);
/* KLine Options Function ----------------*/
void KBeginSettings(void);
void KEndSettings(void);
uint8_t KCommStart(KStartType type);
void KSendData(uint8_t *data, size_t length);
void KSendByte(uint8_t data);



#endif // __K_LINE_H__
