/**********************************************************************
* $Id$		client_parser.c				2013-07-08
*//**
* @file		client_parser.c
* @brief	Deal With Client Command Source Code(Command Parser part)
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

/* Private Type Definition ---------------------------------------------------- */
ClientParserStruct client_parser;
 
/*********************************************************************//**
 * @brief 		Get Version to Client.
 * @param[in] 	None
 * @return 		None
 ***********************************************************************/
static void CfgExeGetVersion(uint8_t *cmd, uint16_t length)
{
    SendToClientPack(client_parser.version, sizeof(client_parser.version));
}

/*********************************************************************//**
 * @brief 		Configuration for set heart beat.
 * @param[in] 	cmd     heart beat command.
 * @param[in]   length  heart beat length.
 * @return 		None
 ***********************************************************************/
static void CfgExeSetHeartBeat(uint8_t *cmd, uint16_t length)
{
    memcpy(client_obd.heart_beat, cmd + 1, length - 1);
}

/*********************************************************************//**
 * @brief 		Configuration to exit obd mode
 * @param[in] 	cmd     command buff.
 * @param[in]   length  command buff length.
 * @return 		None
 ***********************************************************************/
static void CfgExeExitOBD(uint8_t *cmd, uint16_t length)
{
    client_parser.mode = CLIENT_CFG_MODE;
    SendToClientBytePack(CLIENT_ERR_NONE);
}

/*********************************************************************//**
 * @brief 		Configuration executation.
 * @param[in] 	cmd     command buff.
 * @param[in]   length  command buff length.
 * @return 		None
 ***********************************************************************/
static void CfgExe(uint8_t *cmd, uint16_t length)
{
    /* cmd[0] is the index of Cfg funciton
     * so we skip cmd[0]
     */
    client_parser.exe[cmd[0]](cmd + 1, length - 1);
}

/*********************************************************************//**
 * @brief 		Configuration Parser Header 1.
 * @param[in] 	b   commmand byte.
 * @return 		None
 ***********************************************************************/
static void CfgParserHeader1(uint8_t *b)
{
    if (*b != CLIENT_HEADER_1) {
        SendToClientBytePack(CLIENT_ERR_HEADER_1);
    } else {
        client_parser.data.current = CLIENT_RECV_HEADER_2;
        client_parser.data.checksum = *b;
    }
}

/*********************************************************************//**
 * @brief 		Configuration Parser Header 2.
 * @param[in] 	b   command byte
 * @return 		None
 ***********************************************************************/
static void CfgParserHeader2(uint8_t *b)
{
    if (*b != CLIENT_HEADER_2) {
        SendToClientBytePack(CLIENT_ERR_HEADER_2);
        client_parser.data.current = CLIENT_RECV_HEADER_1;
    } else {
        client_parser.data.current = CLIENT_RECV_LENGTH_1;
        client_parser.data.checksum ^= *b;
    }
}

/*********************************************************************//**
 * @brief 		Configuration Parser Length 1
 * @param[in] 	b   command byte
 * @return 		None
 ***********************************************************************/
static void CfgParserLength1(uint8_t *b)
{
    client_parser.data.length = *b << 8;
    client_parser.data.current = CLIENT_RECV_LENGTH_2;
    client_parser.data.checksum ^= *b;
}

/*********************************************************************//**
 * @brief 		Configuration Parser Length 2
 * @param[in] 	b   command byte
 * @return 		None
 ***********************************************************************/
static void CfgParserLength2(uint8_t *b)
{
    client_parser.data.length += *b;
    if (client_parser.data.length > 256) {
        SendToClientBytePack(CLIENT_ERR_DATA_LENGTH);
        client_parser.data.current = CLIENT_RECV_HEADER_1;
    } else {
        client_parser.data.current = CLIENT_RECV_DATA;
        client_parser.data.checksum ^= *b;
        client_parser.data.index = 0;
    }
}

/*********************************************************************//**
 * @brief 		Configuration Parser data
 * @param[in] 	b   command byte
 * @return 		None
 ***********************************************************************/
static void CfgParserData(uint8_t *b)
{
    client_parser.data.data[client_parser.data.index++] = *b;
    client_parser.data.checksum ^= *b;
    if (client_parser.data.index == client_parser.data.length)
        client_parser.data.current = CLIENT_RECV_CHECKSUM;
}

/*********************************************************************//**
 * @brief 		Configuration Parser checksum
 * @param[in] 	b   command byte.
 * @return 		None
 ***********************************************************************/
static void CfgParserChecksum(uint8_t *b)
{
    if (client_parser.data.checksum != *b || 
        (client_parser.data.length == 0)) {
        SendToClientBytePack(CLIENT_ERR_CHECKSUM);
    } else {
        CfgExe(client_parser.data.data, client_parser.data.length);
    }
    client_parser.data.current = CLIENT_RECV_HEADER_1;
}

/*********************************************************************//**
 * @brief 		Configuration Parser
 * @param[in] 	b   command byte
 * @return 		None
 ***********************************************************************/
static void CfgParser(uint8_t *b)
{
    client_parser.cfg[client_parser.data.current](b);
}

/*********************************************************************//**
 * @brief 		OBD Command Parser
 * @param[in] 	b   command byte
 * @return 		None
 ***********************************************************************/
static void ObdParser(uint8_t *b)
{
	static size_t index = 0;
	static uint8_t buff[128];

    // save receive byte to buff[index] for future use
    // where index is the receive command sequence No.
	buff[index] = *b;
    
    // the exit is the command to exit obd mode.
    // we compare it to receive buff, one by one.
	if (client_parser.exit[index] == buff[index]) {
		index++;
        // if index reach to exit size,
        // here receive command exactly equal to exit
        // we just reset to cfg mode.
		if (index == sizeof(client_parser.exit)) {
			int32_t i;
			for (i = 0; i < index; i++) {
				CfgParser(client_parser.exit + i);
			}
			index = 0;
		}
		return;
	} else {
		// Execute buff & length = (index + 1) data to ECU. then..
        client_obd.send(buff, index + 1);
        SendToClientNoPack(buff, index + 1);
		index = 0;
	}
}

/*********************************************************************//**
 * @brief 		Client Hardware Initialize
 * @param[in] 	None
 * @return 		None
 ***********************************************************************/
void ClientHardwareInit(void)
{
	// UART Configuration structure variable
	UART_CFG_Type uart_config;
	// UART FIFO Configuration Struct variable
	UART_FIFO_CFG_Type uart_fifo;
	// Pin configuration for UART0
	PINSEL_CFG_Type pin;
    
    // Initialize UART0 pin connect
	pin.Funcnum = PINSEL_FUNC_1;
	pin.OpenDrain = PINSEL_PINMODE_OPENDRAIN;
	pin.Pinmode = PINSEL_PINMODE_PULLUP;
	pin.Pinnum = PINSEL_PIN_2;
	pin.Portnum = PINSEL_PORT_0;
	PINSEL_ConfigPin(&pin);
	pin.Pinnum = PINSEL_PIN_3;
	PINSEL_ConfigPin(&pin);

	// Initialize UART Configration parameter structure to default state:
	// Baudrate = 9600bps
	// 8 data bit
	// 1 stop bit
	// None parity
	UART_ConfigStructInit(&uart_config);

	// Set baudrate to 115200 */
	uart_config.Baud_rate = 115200;
	// Initialize UART0 peripheral with give to corresponding parameter */
	UART_Init((LPC_UART_TypeDef *)LPC_UART0, &uart_config);

	// Initialize FIFOConfigStruct to default state:
	// 				- FIFO_DMAMode = DISABLE
	// 				- FIFO_Level = UART_FIFO_TRGLEV0
	// 				- FIFO_ResetRxBuf = ENABLE
	// 				- FIFO_ResetTxBuf = ENABLE
	// 				- FIFO_State = ENABLE
	UART_FIFOConfigStructInit(&uart_fifo);
    
	// Initialize FIFO for UART0 peripheral
	UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART0, &uart_fifo);

	// Enable UART Transmit
	UART_TxCmd((LPC_UART_TypeDef *)LPC_UART0, ENABLE);

	// Enable UART Rx interrupt
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RBR, ENABLE);

	// Enable UART line status interrupt
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART0, UART_INTCFG_RLS, ENABLE);
    
	// preemption = 1, sub-priority = 1
	NVIC_SetPriority(UART0_IRQn, ((0x01 << 3) | 0x01));
    
	// Enable Interrupt for UART0 channel
	NVIC_EnableIRQ(UART0_IRQn);
}

/*********************************************************************//**
 * @brief 		Client Field Initialize
 * @param[in] 	None
 * @return 		None
 ***********************************************************************/
void ClientParserStructInit(void)
{
	// Reset from client ring buffer
	RingBufferInit(&(client_parser.buffer));

	// Initialize events
    client_parser.event = OSMboxCreate(NULL);

	// CMD Parse
    client_parser.mode = CLIENT_CFG_MODE;
    
    client_parser.data.current = CLIENT_RECV_HEADER_1;
	client_parser.parser[CLIENT_CFG_MODE] = CfgParser;
	client_parser.parser[CLIENT_OBD_MODE] = ObdParser;
    
    client_parser.cfg[CLIENT_RECV_HEADER_1] = CfgParserHeader1;
    client_parser.cfg[CLIENT_RECV_HEADER_2] = CfgParserHeader2;
    client_parser.cfg[CLIENT_RECV_LENGTH_1] = CfgParserLength1;
    client_parser.cfg[CLIENT_RECV_LENGTH_2] = CfgParserLength2;
    client_parser.cfg[CLIENT_RECV_DATA] = CfgParserData;
    client_parser.cfg[CLIENT_RECV_CHECKSUM] = CfgParserChecksum;
    
    client_parser.exe[CLIENT_CFG_GET_VERSION] = CfgExeGetVersion;
    client_parser.exe[CLIENT_CFG_OBD_MODE] = CfgExeOBDMode;
    client_parser.exe[CLIENT_CFG_SET_HEART_BEAT] = CfgExeSetHeartBeat;
    client_parser.exe[CLIENT_CFG_EXIT_OBD] = CfgExeExitOBD;
    
    client_parser.version[0] = 'D';
    client_parser.version[1] = 'A';
    client_parser.version[2] = '-';
    client_parser.version[3] = 'V';
    client_parser.version[4] = '0';
    client_parser.version[5] = '.';
    client_parser.version[6] = '0';
    client_parser.version[7] = '.';
    client_parser.version[8] = '1';
    client_parser.version[9] = '\0';
}

/*********************************************************************//**
 * @brief 		Client Parer uc/OS II task
 * @param[in] 	p_arg parameter
 * @return 		None
 ***********************************************************************/
void ClientParserTask(void *p_arg)
{
    uint8_t b;
    INT8U err = OS_ERR_NONE;
    (void) p_arg; // avoid compile warning
    
    // Start Client Parser
    for (;;) {
        OSMboxPend(client_parser.event, 500, &err);
        if (err == OS_ERR_NONE) {
            while (!RingBufferIsEmpty(&(client_parser.buffer))) {
                RingBufferPop(&(client_parser.buffer), &b);
                client_parser.parser[client_parser.mode](&b);
            }
        }
 	}
}
