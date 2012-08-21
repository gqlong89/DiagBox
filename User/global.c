/**********************************************************************
* $Id$		global.c				2012-07-26
*//**
* @file		global.c
* @brief	The Global Variables
* @version	1.0
* @date		25. July. 2012
* @author	Rocky Weshare Xu
*
* Copyright(C) 2012, NCS
* All rights reserved.
*
**********************************************************************/

#include "includes.h"

RingBuffer from_client_buffer;
RingBuffer to_client_buffer;
RingBuffer ecu_buffer;
OS_EVENT *from_client_event;
OS_EVENT *to_client_event;
OS_EVENT *ecu_receive_event;
uint8_t keep_link_buffer[16];
