/**********************************************************************
* $Id$		ringbuffer.h				2012-07-25
*//**
* @file		ringbuffer.h
* @brief	The Ring Buffer Structure
* @version	1.0
* @date		28. July. 2012
* @author	Rocky Weshare Xu
*
* Copyright(C) 2012, DNT Diagnostic And Tools
* All rights reserved.
*
**********************************************************************/

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdlib.h>
#include <stdint.h>
#include <core_cm3.h>

/* Buffer size definition */
#define RING_BUFSIZE 256

/** @brief Ring buffer structure */
typedef struct
{
  __IO uint32_t head; /*!< Ring buffer head index */
  __IO uint32_t tail; /*!< Ring buffer tail index */
  __IO uint8_t  buff[RING_BUFSIZE]; /*!< Data ring buffer */
} RingBuffer;

/* Buf Mask */
#define RING_BUFFER_MASK (RING_BUFSIZE - 1)
/* Check buf is full or not */
#define RingBufferIsFull(p) (((p)->tail & RING_BUFFER_MASK) == (((p)->head + 1) & RING_BUFFER_MASK))
/* Check buf will be full in next receiving or not */
#define RingBufferWillFull(p) (((p)->tail & RING_BUFFER_MASK) == (((p)->head + 2) & RING_BUFFER_MASK))
/* Check buf is empty */
#define RingBufferIsEmpty(p) (((p)->head & RING_BUFFER_MASK) == ((p)->tail & RING_BUFFER_MASK))
/* Reset buf */
void RingBufferReset(RingBuffer *p);
int32_t RingBufferPush(RingBuffer *p, uint8_t b);
int32_t RingBufferPop(RingBuffer *p, uint8_t *b);
void RingBufferInit(RingBuffer *p);


#endif /* __RING_BUFFER_H__ */
