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
*		          2.  Exit OBD            : 55 AA 00 01 FF 01
*                	Response     			  : 55 AA 00 01 00 01
*             3.  Set Keep Link       : 55 AA 00 0x FE xx xx xx xx.. CS
*                 Response            : 55 AA 00 01 00 01
*				  
* @version	1.0
* @date		25. July. 2012
* @author	Rocky Weshare Xu
*
* Copyright(C) 2012, NCS
* All rights reserved.
*
**********************************************************************/

#include "includes.h"

static int32_t running_mode = CFG_MODE;
static int32_t current_cfg_byte = RECV_HEADER_1;
static OS_STK client_parser_stk[APP_TASK_CLIENT_PARSER_STK_SIZE];
static CommandParser command_parser[2];
static uint8_t version[] = {0x44, 0x41, 0x2D, 0x56, 0x30, 0x2E, 0x30, 0x2E, 0x31, 0x00};
static uint8_t exit_obd[] = {0x55, 0xAA, 0x00, 0x01, 0xFF, 0x01};

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		UART Line Status Error
 * @param[in]	err_type	UART Line Status Error Type
 * @return		None
 **********************************************************************/
static void UartIntErr(uint8_t err_type) {
	uint8_t test;
	test = test;
 	// Loop forever
 	for (;;) {
 		test = err_type;
 	}
 }

/********************************************************************//**
 * @brief 		UART receive function (ring buffer used)
 * @param[in]	None
 * @return 		None
 *********************************************************************/
static void UartIntReceive(void) {
	uint8_t tmpc;
	uint32_t len;

	for (;;) {
		// Call UART read function in UART driver 
		len = UART_Receive((LPC_UART_TypeDef*)LPC_UART0, &tmpc, 1, NONE_BLOCKING);

		// If data receive
		if (len) {
			RingBufferPush(&from_client_buffer, tmpc);
		} else {
			// No more data
			OSMboxPost(from_client_event, (void*)1);
			break;
		}
	}
}

/*********************************************************************//**
 * @brief		UART0 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void UART0_IRQHandler(void) {
	uint32_t intsrc, tmp, tmp1;

	// Determine the interrupt source
	intsrc = UART_GetIntId(LPC_UART0);
	tmp = intsrc & UART_IIR_INTID_MASK;

	// Receive Line Status
	if (tmp == UART_IIR_INTID_RLS) {
		// Check line status
		tmp1 = UART_GetLineStatus(LPC_UART0);
		// Mask out the receive ready and transmit holding empty status
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_BI | UART_LSR_RXFE);

		// If any error exist
		if (tmp1) {
			UartIntErr(tmp1);
		}
	}

	// Receive data available or character time-out
	if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI)) {
		UartIntReceive();
	}

	// Transmit Holding Empty
	if (tmp == UART_IIR_INTID_THRE) {

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*********************************************************************//**
 * @brief		client parser
 * @param[in]	None
 * @return 		None
 **********************************************************************/
 static void ClientParser(void *p_arg) {
 	uint8_t b;
 	INT8U err = OS_ERR_NONE;
 	RingBuffer *rb = &from_client_buffer;

 	(void)p_arg;

 	for (;;) {
 		OSMboxPend(from_client_event, 500, &err);
 		if (err == OS_ERR_NONE) {
 			while (!RingBufferIsEmpty(rb)) {
 				RingBufferPop(rb, &b);
 				command_parser[running_mode](&b);
 			}
 		}
 	}
 }

 /*********************************************************************//**
 * @brief		command response
 * @param[in]	command data buffer
 * @return 		None
 **********************************************************************/
static void ClientResponse(uint8_t *buff, uint16_t len) {
	uint8_t checksum = 0;
	uint8_t header_1 = HEADER_1;
	uint8_t header_2 = HEADER_2;
	uint8_t length_1 = (len >> 8) & 0xFF;
	uint8_t length_2 = len & 0xFF;
	uint16_t i;


	checksum ^= header_1;
	checksum ^= header_2;
	checksum ^= length_1;
	checksum ^= length_2;

	for (i = 0; i < len; i++) {
		checksum ^= buff[i];
	}

	UART_Send((LPC_UART_TypeDef*)LPC_UART0, &header_1, 1, BLOCKING);
	UART_Send((LPC_UART_TypeDef*)LPC_UART0, &header_2, 1, BLOCKING);
	UART_Send((LPC_UART_TypeDef*)LPC_UART0, &length_1, 1, BLOCKING);
	UART_Send((LPC_UART_TypeDef*)LPC_UART0, &length_2, 1, BLOCKING);
	UART_Send((LPC_UART_TypeDef*)LPC_UART0, buff, len, BLOCKING);
	UART_Send((LPC_UART_TypeDef*)LPC_UART0, &checksum, 1, BLOCKING);
}

/*********************************************************************//**
 * @brief		command execute
 * @param[in]	Execute correct command.
 * @return 		None
 **********************************************************************/
static void CfgExe(uint8_t *cmd, uint16_t length) {
	uint8_t err = ERR_NONE;
	switch(cmd[0]) {
	case CFG_GET_VERSION:
		ClientResponse(version, sizeof(version));
		break;
	case CFG_K_LINE:
		running_mode = OBD_MODE;
		ClientResponse(&err, 1);
		break;
	case CFG_SET_KEEP_LINK_CMD:
		memcpy(keep_link_buffer, cmd + 1, length - 1);
		ClientResponse(&err, 1);
		break;
	case CFG_EXIT_OBD:
		running_mode = CFG_MODE;
		ClientResponse(&err, 1);
		break;
	}
}

/*********************************************************************//**
 * @brief		command parser
 * @param[in]	Receive buffer pointer one byte.
 * @return 		None
 **********************************************************************/
static void CfgParser(uint8_t *b) {
  static uint16_t length = 0;
  static uint16_t i = 0;
  static uint8_t data[256];
  static uint8_t checksum = 0;
  
  switch(current_cfg_byte) {
  case RECV_HEADER_1:
    if (*b != HEADER_1) {
      uint8_t temp = ERR_HEADER_1;
      ClientResponse(&temp, 1);
    } else {
      current_cfg_byte = RECV_HEADER_2;
      checksum = *b;
    }
    break;
  case RECV_HEADER_2:
    if (*b != HEADER_2) {
      uint8_t temp = ERR_HEADER_2;
      ClientResponse(&temp, 1);
      current_cfg_byte = RECV_HEADER_1;
    } else {
      current_cfg_byte = RECV_LENGTH_1;
      checksum ^= *b;
    }
    break;
  case RECV_LENGTH_1:
    length = *b << 8;
    current_cfg_byte = RECV_LENGTH_2;
    checksum ^= *b;
    break;
  case RECV_LENGTH_2:
    length += *b;
    if (length > 256) {
      uint8_t temp = ERR_DATA_LENGTH;
      ClientResponse(&temp, 1);
      current_cfg_byte = RECV_HEADER_1;
    } else {
      current_cfg_byte = RECV_DATA;
      checksum ^= *b;
      i = 0;
    }
    break;
  case RECV_DATA:
    data[i++] = *b;
    checksum ^= *b;
    if (i == length)
      current_cfg_byte = RECV_CHECKSUM;
    break;
  case RECV_CHECKSUM:
    if (checksum != *b) {
      uint8_t temp = ERR_CHECKSUM;
      ClientResponse(&temp, 1);
    } else {
      CfgExe(data, length);
    }
    current_cfg_byte = RECV_HEADER_1;
    break;
  }
}

/*********************************************************************//**
 * @brief		obd parser
 * @param[in]	parser OBD command.
 * @return 		None
 **********************************************************************/
static void ObdParser(uint8_t *b) {
	static int32_t index = 0;
	static uint8_t buff[128];
	buff[index] = *b;
	if (exit_obd[index] == buff[index]) {
		index++;
		if (index == sizeof(exit_obd)) {
			int32_t i;
			for (i = 0; i < index; i++) {
				CfgParser(exit_obd + i);
			}
			index = 0;
		}
		return;
	} else {
		// Execute buff & length = (index + 1) data to ECU. then..
		index = 0;
	}
}

/*********************************************************************//**
 * @brief		client task create
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void ClientTaskCreate(void) {
	// UART Configuration structure variable
	UART_CFG_Type uart_config;
	/* UART FIFO Configuration Struct variable */
	UART_FIFO_CFG_Type uart_fifo;
	/* Pin configuration for UART0 */
	PINSEL_CFG_Type pin;
	/* ucos variables */
	INT8U err;

	/*
	 * Initialize UART0 pin connect
	 */
	pin.Funcnum = PINSEL_FUNC_1;
	pin.OpenDrain = PINSEL_PINMODE_OPENDRAIN;
	pin.Pinmode = PINSEL_PINMODE_PULLUP;
	pin.Pinnum = PINSEL_PIN_2;
	pin.Portnum = PINSEL_PORT_0;
	PINSEL_ConfigPin(&pin);
	pin.Pinnum = PINSEL_PIN_3;
	PINSEL_ConfigPin(&pin);

	/* Initialize UART Configration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&uart_config);

	/* Set baudrate to 115200 */
	uart_config.Baud_rate = 115200;
	/* Initialize UART0 peripheral with give to corresponding parameter */
	UART_Init((LPC_UART_TypeDef *)LPC_UART0, &uart_config);

	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&uart_fifo);

	/* Initialize FIFO for UART0 peripheral */
	UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART0, &uart_fifo);

	/* Enable UART Transmit */
	UART_TxCmd((LPC_UART_TypeDef *)LPC_UART0, ENABLE);

	/* Enable UART Rx interrupt */
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RBR, ENABLE);

	/* Enable UART line status interrupt */
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RLS, ENABLE);

	/* Reset to client ring buffer and from client ring buffer */
	RingBufferReset(&from_client_buffer);

	/* preemption = 1, sub-priority = 1*/
	NVIC_SetPriority(UART0_IRQn, ((0x01 << 3) | 0x01));
	/* Enable Interrupt for UART0 channel */
	NVIC_EnableIRQ(UART0_IRQn);

	/* Initialize events */
	from_client_event = OSMboxCreate(NULL);

	/* CMD Parse */
	err = OSTaskCreateExt((void (*)(void *)) ClientParser,
		(void          * ) 0,
		(OS_STK        * )&client_parser_stk[APP_TASK_CLIENT_PARSER_STK_SIZE - 1],
		(INT8U           ) APP_TASK_CLIENT_PARSER_PRIO,
		(INT16U          ) APP_TASK_CLIENT_PARSER_PRIO,
		(OS_STK        * )&client_parser_stk[0],
		(INT32U          ) APP_TASK_CLIENT_PARSER_STK_SIZE,
		(void          * )0,
		(INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

	if (err != OS_ERR_NONE) {
		return;
	}

	command_parser[0] = CfgParser;
	command_parser[1] = ObdParser;

	#if OS_TASK_NAME_EN > 0
	OSTaskNameSet(APP_TASK_CLIENT_PARSER_PRIO, (CPU_INT08U *)"Client Parser Task", &err);
	#endif
}

