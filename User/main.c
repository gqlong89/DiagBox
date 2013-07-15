/*********************************************************************
* $Id$		main.c				2012-07-26
*//**
* @file		main.c
* @brief	The main source code
* @version	1.0
* @date		25. July. 2012
* @author	Rocky Weshare Xu
*
* Copyright(C) 2013, DNT Diagnostic And Tools
* All rights reserved.
*
**********************************************************************/

/* Includes ------------------------------------------------------------------- */
#include "includes.h"

/* Private Variables ---------------------------------------------------------- */
static OS_STK main_stk[APP_TASK_START_STK_SIZE];
/* Private Functions ---------------------------------------------------------- */
/*********************************************************************//**
 * @brief 		Main task, just initialize all need.
 * @param[in] 	None
 * @return 		None
 ***********************************************************************/
static void MainTask(void *p_arg)
{
    CPU_INT08U os_err;
    
    (void) p_arg;
    (void) os_err;
    
    /* Initialize the SysTick */
    OS_CPU_SysTickInit(SystemCoreClock / 1000);
    
    #if OS_TASK_STAT_EN > 0
    OSStatInit();
    #endif
    
    // Hardware Initialize
    ClientHardwareInit();
    KHardwareInit();
    
    ClientParserStructInit();
    ClientResponseInit();
    CfgExeOBDInit();
    KConfigInit();
    
    os_err = ClientParserTaskCreate();
    os_err = ClientResponseTaskCreate();
    
    while (1) {
        OSTimeDlyHMSM(1, 0, 0, 0); // Delay One Hour.
    }
}

/*-------------------------MAIN FUNCTION------------------------------*/
int32_t main(void)
{
    CPU_INT08U os_err;
    
    os_err = os_err;
    
    SystemInit();
    SystemCoreClockUpdate();
    // Note: disable all interrupt before OS running
    IntDisAll();  // Disable all ints until we are ready to accept them.
    OSInit(); // Initialize "uC/OS-II, The Real-Time Kernel".


    os_err = OSTaskCreateExt((void(*)(void*)) MainTask, // Create main task
                             (void *) 0, 
                             (OS_STK *) &main_stk[APP_TASK_START_STK_SIZE - 1],
                             (INT8U) APP_TASK_START_PRIO,
                             (INT16U) APP_TASK_START_PRIO,
                             (OS_STK*) &main_stk[0],
                             (INT32U) APP_TASK_START_STK_SIZE,
                             (void*) 0,
                             (INT16U) (OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    
    #if OS_TASK_NAME_EN > 0
    OSTaskNameSet(APP_TASK_START_PRIO, (CPU_INT08U*) "Main Task", &os_err);
    #endif
    
    OSStart(); // Start multitasking (i.e, give contorl to uC/OS-II).
    
    return 0;
}

