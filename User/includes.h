/**********************************************************************
* $Id$		includes.h			2013-07-09
*//**
* @file		includes.h
* @brief	Includes file for DiagBox
* @version	1.0
* @date		09 Jul. 2013
* @author	Rocky Weshare Xu
*
* Copyright(C) 2013, DNT Diagnostic And Tools.
* All rights reserved.
*
**********************************************************************/
#ifndef __INCLUDES_H__
#define __INCLUDES_H__

/* Includes ------------------------------------------------------------------- */
/*********************************************************************//**
 * Standard C Headers
 **********************************************************************/
#include <stdint.h>
#include <string.h>

/*********************************************************************//**
 * NXP LPC 17xx Drivers Headers
 **********************************************************************/
#include <system_LPC17xx.h>
#include <lpc17xx.h>
#include <lpc17xx_gpio.h>
#include <lpc17xx_uart.h>
#include <lpc17xx_pinsel.h>
#include <lpc17xx_gpdma.h>
#include <lpc17xx_exti.h>
#include <lpc17xx_timer.h>
#include <core_cm3.h>

/*********************************************************************//**
 * uc/OS II Headers
 **********************************************************************/
#include <ucos_ii.h>
#include <cpu.h>

/*********************************************************************//**
 * DiagBox Ring Buffer Headers
 **********************************************************************/
#include "ringbuffer.h"

/*********************************************************************//**
 * DiagBox Client Function Headers
 **********************************************************************/
#include "client.h"
#include "client_parser.h"
#include "client_response.h"

/*********************************************************************//**
 * OBD Function Headers
 **********************************************************************/
#include "obd_exe.h"
#include "kline.h"

/*********************************************************************//**
 * DiagBox Support ECU Manufactor Headers
 **********************************************************************/
#include "mikuni.h"



#endif // __INCLUDES_H__
