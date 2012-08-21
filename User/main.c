/**********************************************************************
* $Id$		main.c				2012-07-26
*//**
* @file		main.c
* @brief	The main source code
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

/************************** PRIVATE VARIABLES *************************/
static OS_STK task_start_stk[APP_TASK_START_STK_SIZE];
static void AppTaskStart(void *p_arg);

int32_t main(void) {
	CPU_INT08U os_err;
	os_err = os_err; // Prevent warning...

	// Note: disable all interrupt before OS running
	IntDisAll(); // Disable all ints until we are ready to accept them.

	OSInit(); // Initialize "uC/OS-II, The Real-Time Kernel".

	os_err = OSTaskCreateExt((void(*)(void*))AppTaskStart, // Create the start task.
		(void *)0,
		(OS_STK *) &task_start_stk[APP_TASK_START_STK_SIZE - 1],
		(INT8U) APP_TASK_START_PRIO,
		(INT16U) APP_TASK_START_PRIO,
		(OS_STK *) &task_start_stk[0],
		(INT32U) APP_TASK_START_STK_SIZE,
		(void *)0,
		(INT16U)(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

	#if OS_TASK_NAME_EN > 0
	OSTaskNameSet(APP_TASK_START_PRIO, (CPU_INT08U *)"Start Task", &os_err);
	#endif

	OSStart(); // Start multiasking (i.e. give control to uC/OS-II).

	return 0;
}

static void AppTaskStart(void *p_arg) {
	(void)p_arg;

	OS_CPU_SysTickInit(SystemCoreClock / 1000); // Initialize the SysTick

	#if OS_TASK_STAT_EN > 0
	OSStatInit();
	#endif

	ClientTaskCreate();

	for (;;) {
		OSTimeDlyHMSM(0, 1, 0, 0); // Delay One Minute.
	}
}

