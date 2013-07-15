/**********************************************************************
* $Id$		client_response.c				2013-07-08
*//**
* @file		client_response.c
* @brief	Deal With Client Command Source Code(Response part)
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
*		      2.  Exit OBD            : 55 AA 00 01 04 FA
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

/* Public Variables ---------------------------------------------------------- */
ClientResponseStruct client_res;

/*********************************************************************//**
 * @brief 		Client Response Function
 * @param[in] 	buff:   response data buff
 * @param[in]	len:    response data buff length
 * @return 		None
 ***********************************************************************/
void SendToClientPack(uint8_t *buff, size_t len)
{
	size_t i;
    uint8_t cs = 0;
        
    RingBufferPush(&client_res.buffer, CLIENT_HEADER_1);
    cs ^= CLIENT_HEADER_1;
    
    RingBufferPush(&client_res.buffer, CLIENT_HEADER_2);
    cs ^= CLIENT_HEADER_2;
    
    RingBufferPush(&client_res.buffer, (len >> 8) & UART_THR_MASKBIT);
    cs ^= (len >> 8) & UART_THR_MASKBIT;
    
    RingBufferPush(&client_res.buffer, len & UART_THR_MASKBIT);
    cs ^= len & UART_THR_MASKBIT;
    
    for (i = 0; i < len; i++) {
        RingBufferPush(&client_res.buffer, buff[i]);
        cs ^= buff[i];
    }
    
    RingBufferPush(&client_res.buffer, cs);
    OSMboxPost(client_res.event, (void*) 1);
}

/*********************************************************************//**
 * @brief 		Client Response One Byte
 * @param[in] 	byte:   is the byte will be response
 * @return 		None
 ***********************************************************************/
void SendToClientBytePack(uint8_t byte)
{
    uint8_t cs = 0;
    
    RingBufferPush(&client_res.buffer, CLIENT_HEADER_1);
    cs ^= CLIENT_HEADER_1;
    
    RingBufferPush(&client_res.buffer, CLIENT_HEADER_2);
    cs ^= CLIENT_HEADER_2;
    
    RingBufferPush(&client_res.buffer, 0);
    cs ^= 0;
    
    RingBufferPush(&client_res.buffer, 1);
    cs ^= 1;
    
    RingBufferPush(&client_res.buffer, byte);
    cs ^= byte;
    
    RingBufferPush(&client_res.buffer, cs);
    OSMboxPost(client_res.event, (void *) 1);
}

/*********************************************************************//**
 * @brief 		Client Echo (won't pack)
 * @param[in] 	buff:   echo data buff
 * @param[in]	len:    echo data buff length
 * @return 		None
 ***********************************************************************/
void SendToClientNoPack(uint8_t *buff, size_t len)
{
    size_t i;
    for (i = 0; i < len; i++) {
        RingBufferPush(&client_res.buffer, buff[i]);
    }
    OSMboxPost(client_res.event, (void *) 1);
}

/*********************************************************************//**
 * @brief 		Client Echo One Byte (won't pack)
 * @param[in] 	byte:   echo data
 * @return 		None
 ***********************************************************************/
void SendToClientByteNoPack(uint8_t byte)
{
    RingBufferPush(&client_res.buffer, byte);
    OSMboxPost(client_res.event, (void *) 1);
}

/*********************************************************************//**
 * @brief       Client Response Initialize
 * @param[in]   None.
 * @return      None.
 *********************************************************************/
void ClientResponseInit(void)
{
	// Reset to client ring buffer
	RingBufferInit(&client_res.buffer);

	// Initialize events
	client_res.event = OSMboxCreate(NULL);
    
    client_res.is_response = false;
}

/*********************************************************************//**
 * @bried       Pop ringbuffer and send to client.
 * @param[in]   None
 * @return      None
 *********************************************************************/
void SendToClient(void)
{
    uint8_t b;
    while (!RingBufferIsEmpty(&client_res.buffer)) {
        RingBufferPop(&client_res.buffer, &b);
        LPC_UART0->THR = b & UART_THR_MASKBIT;
    }
}

/*********************************************************************//**
 * @brief 		Client Response uc/OS II task
 * @param[in] 	p_arg parameter
 * @return 		None
 ***********************************************************************/
void ClientResponseTask(void *p_arg)
{
    INT8U err = OS_ERR_NONE;
    (void) p_arg; // avoid compile warning
        
    // Start Client Parser
    ClientResponseRun(true);
    for (;;) {
        if (client_res.is_response) {
            OSMboxPend(client_res.event, 10, &err);
            if (err == OS_ERR_NONE) {
                SendToClient();
            }
        } else {
            OSTimeDlyHMSM(0, 0, 0, 10);
        }
 	}
}

/*******************************************************************//**
 * @brief       Client Response Start or Stop
 * @param[in]   None
 * @return      None
 *******************************************************************/
void ClientResponseRun(bool is_run)
{
    client_res.is_response = is_run;
    SendToClient();
}


