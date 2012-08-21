/**********************************************************************
* $Id$		global.h				2012-07-25
*//**
* @file		global.h
* @brief	The Global Variables
* @version	1.0
* @date		28. July. 2012
* @author	Rocky Weshare Xu
*
* Copyright(C) 2012, NCS
* All rights reserved.
*
**********************************************************************/

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

/***********************Global Variables**************************************/
extern RingBuffer from_client_buffer;
extern RingBuffer to_client_buffer;
extern RingBuffer ecu_buffer;
extern OS_EVENT *from_client_event;
extern OS_EVENT *to_client_event;
extern OS_EVENT *ecu_receive_event;
extern uint8_t keep_link_buffer[16];


#endif /* __GLOBAL_H__ */
