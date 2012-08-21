/**********************************************************************
* $Id$		ringbuffer.c				2012-07-26
*//**
* @file		ringbuffer.c
* @brief	The Ring Buffer Implement
* @version	1.0
* @date		25. July. 2012
* @author	Rocky Weshare Xu
*
* Copyright(C) 2012, NCS
* All rights reserved.
*
**********************************************************************/

/* Includes ------------------------------------------------------------------- */
#include "includes.h"

#define RingBufferIncr(idx) (idx = (idx + 1) & RING_BUFFER_MASK)

void RingBufferReset(RingBuffer *p)
{
  if (p == NULL)
    return;
  
  p->head = 0;
  p->tail = 0;
}

int32_t RingBufferPush(RingBuffer *p, uint8_t b)
{
  if (RingBufferWillFull(p))
    return -1;
  
  p->buff[p->head] = b;
  RingBufferIncr(p->head);
  return 0;
}

int32_t RingBufferPop(RingBuffer *p, uint8_t *b)
{
  if (RingBufferIsEmpty(p))
    return -1;
  
  *b = p->buff[p->tail];
  RingBufferIncr(p->tail);
  return 0;
}

void RingBufferInit(RingBuffer *p)
{
  if (p == NULL)
    return;
  
  p->head = 0;
  p->tail = 0;
  memset((void*)p->buff, 0, RING_BUFSIZE);
}
