/**********************************************************************
* $Id$		kline.h				2012-08-12
*//**
* @file		kline.h
* @brief	The Ring Buffer Structure
* @version	1.0
* @date		28. July. 2012
* @author	Rocky Weshare Xu
*
* Copyright(C) 2012, NCS
* All rights reserved.
*
**********************************************************************/

#ifndef __K_LINE_H__
#define __K_LINE_H__

#include <stdint.h>

void k_line_init(void);
void k_line_uart_config(uint8_t databits,
                        uint8_t stopbits,
                        uint8_t parity,
                        uint8_t flow_control,
                        uint8_t baudrate);
void k_line_obd_config(uint8_t k_line,
                       uint8_t l_line,
                       uint8_t comm_mode,
                       uint8_t init_mode);
void k_line_addr_init(uint8_t addr); /* 5 baud init */
void k_line_fast_init(uint8_t up_time, 
                      uint8_t down_time, 
                      uint8_t *msg, 
                      uint16_t length);
void k_line_send(uint8_t *msg, uint16_t length);
void k_line_response(uint8_t msg, uint16_t length);

#endif /* __K_LINE_H__ */
