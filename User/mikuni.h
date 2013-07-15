/**********************************************************************
* $Id$		mikuni.h			2013-07-09
*//**
* @file		mikuni.h
* @brief	Mikuni ECU 
* @version	1.0
* @date		09 Jul. 2013
* @author	Rocky Weshare Xu
*
* Copyright(C) 2013, DNT Diagnostic And Tools
* All rights reserved.
*
**********************************************************************/
#ifndef __MIKUNI_H__
#define __MIKUNI_H__

/* Includes ------------------------------------------------------------------- */
#include <stdint.h>

/*********************************************************************//**
 * Macro defines for MIKUNI ECU
 **********************************************************************/
/** Mikuni module definition */
#define MIKUNI_PCM 0x00

/** Mikuni options definition */
#define MIKUNI_PARITY_NONE 0x00
#define MIKUNI_PARITY_ODD 0x01
#define MIKUNI_PARITY_EVEN 0x02

/* Public Functions ----------------------------------------------------------- */
uint8_t MikuniInit(uint8_t *data, uint16_t length);

#endif // __MIKUNI_H__
