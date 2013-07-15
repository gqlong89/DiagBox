/**********************************************************************
* $Id$		client.c				2012-07-25
*//**
* @file		client.c
* @brief	Deal With Client Source Code
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
* @date		25. July. 2012
* @author	Rocky Weshare Xu
*
* Copyright(C) 2012, DNT Diagnostic And Tools
* All rights reserved.
*
**********************************************************************/

/* Includes ------------------------------------------------------------------- */
#include "includes.h"

/* Private Type Definition ---------------------------------------------------- */

/* Private Variables ---------------------------------------------------------- */
static OS_STK client_parser_stk[APP_TASK_CLIENT_PARSER_STK_SIZE];
static OS_STK client_response_stk[APP_TASK_CLIENT_RESPONSE_STK_SIZE];
/* Private Functions ---------------------------------------------------------- */

/*********************************************************************//**
 * @brief 		Client Parser Task Create
 * @param[in] 	None
 * @return 		uC/OS II error code
 ***********************************************************************/
uint8_t ClientParserTaskCreate(void)
{
    CPU_INT08U parser_os_err;
        
    parser_os_err = OSTaskCreateExt((void(*) (void*))ClientParserTask, // Create the client parser task.
                                    (void *) 0,
                                    (OS_STK *) &client_parser_stk[APP_TASK_CLIENT_PARSER_STK_SIZE - 1],
                                    (INT8U) APP_TASK_CLIENT_PARSER_PRIO,
                                    (INT16U) APP_TASK_CLIENT_PARSER_PRIO,
                                    (OS_STK *) &client_parser_stk[0],
                                    (INT32U) APP_TASK_CLIENT_PARSER_STK_SIZE,
                                    (void *) 0,
                                    (INT16U) (OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

    #if OS_TASK_NAME_EN > 0
    OSTaskNameSet(APP_TASK_CLIENT_PARSER_PRIO, (CPU_INT08U *)"Client Parser Task", &parser_os_err);
    #endif
        
    return parser_os_err;
}

/*********************************************************************//**
 * @brief 		Client Parser Task Create
 * @param[in] 	None
 * @return 		uC/OS II error code
 ***********************************************************************/
uint8_t ClientResponseTaskCreate(void)
{
    CPU_INT08U response_os_err;
    
    response_os_err = OSTaskCreateExt((void(*) (void*))ClientResponseTask, // Create the client response task.
                                      (void *) 0,
                                      (OS_STK *) &client_response_stk[APP_TASK_CLIENT_RESPONSE_STK_SIZE - 1],
                                      (INT8U) APP_TASK_CLIENT_RESPONSE_PRIO,
                                      (INT16U) APP_TASK_CLIENT_RESPONSE_PRIO,
                                      (OS_STK *) &client_response_stk[0],
                                      (INT32U) APP_TASK_CLIENT_RESPONSE_STK_SIZE,
                                      (void *) 0,
                                      (INT16U) (OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
                                      
    #if OS_TASK_NAME_EN > 0
    OSTaskNameSet(APP_TASK_CLIENT_RESPONSE_PRIO, (CPU_INT08U *)"Client Response Task", &response_os_err);
    #endif                            
    
    return response_os_err;
}
