/**********************************************************************
* $Id$		kline.c				2013-07-08
*//**
* @file		kline.c
* @brief	K Line Source Codes
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

/* Private Variables ---------------------------------------------------------- */
static UART_CFG_Type uart_config_struct; /* K using UART to communicate */
static PINSEL_CFG_Type pin_cfg;
static UART_FIFO_CFG_Type uart_fifo_config_struct;
static TIM_TIMERCFG_Type tim_config_struct;
static TIM_CAPTURECFG_Type tim_capture_config_struct;
static EXTI_InitTypeDef exti_config_struct;

#define LLINE_PIN (1 << 7)
#define KLINE_PIN_Tx (1 << 15)
#define KLINE_PIN_Rx (1 << 16)

/* Public Variables ----------------------------------------------------------- */
KConfig k_config;

/*********************************************************************//**
 * @brief 		K Line Hardware Initialize, call it after system initialize
 * @param[in] 	None
 * @return 		None
 ***********************************************************************/
void KHardwareInit(void)
{
    // UART 1 pin config
	pin_cfg.Funcnum = PINSEL_FUNC_1;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
	pin_cfg.Pinnum = PINSEL_PIN_15;
	pin_cfg.Portnum = PINSEL_PORT_0;
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum = PINSEL_PIN_16;
	PINSEL_ConfigPin(&pin_cfg);
	
    // Initialize UART Configuration parameter structure to default state:
    // Baudrate = 9600bps
    // 8 data bit
    // 1 stop bit
    // None parity
	UART_ConfigStructInit(&uart_config_struct);
    
    // Initialize FIFOConfigStruct to default state:
	// 						- FIFO_DMAMode = DISABLE
	//						- FIFO_Level = UART_FIFO_TRGLEV0
	//						- FIFO_ResetRxBuf = ENABLE
	//						- FIFO_ResetTxBuf = ENABLE
	//						- FIFO_State = ENABLE
	UART_FIFOConfigStructInit(&uart_fifo_config_struct);
    
    // Initialize FIFO for UART1 peripheral
    UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART1, &uart_fifo_config_struct);
    
    // set UART1 Rx as EXTI Line
    EXTI_Init();
    exti_config_struct.EXTI_Line = EXTI_EINT1;
    exti_config_struct.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
    exti_config_struct.EXTI_polarity = EXTI_POLARITY_HIGH_ACTIVE_OR_RISING_EDGE;
    EXTI_Config(&exti_config_struct);
    
    // l line pin config
    pin_cfg.Pinnum = PINSEL_PIN_7;
    PINSEL_ConfigPin(&pin_cfg);
}

/*********************************************************************//**
 * @brief       K Line configration initialize
 * @param[in]   None
 * @return      None
 **********************************************************************/
void KConfigInit(void)
{
    k_config.low_time = 25; // only for fast init mode, low signal time duration, ms.
    k_config.high_time = 25; // only for fast init mode, high signal time duratio, ms.
    k_config.bit_time = 0; // only for 5 baud init, receive 0x55 time.
    k_config.bit_count = 0; // only for 5 baud init, receive 0x55 bit count;
    k_config.baudrate = 10416;
    k_config.parity = K_PARITY_NONE;
    k_config.flow_control = K_FLOWCONTROL_NONE;
    k_config.stopbits = K_STOPBITS_ONE;
    k_config.baudrate_event = OSMboxCreate(NULL);
    k_config.databits = 8;
    k_config.l_line = 0; // 0 for no l line config.
    k_config.k_line = 0; // 0 for no meaning.
    k_config.addr_code = 0x33; // only for 5 baud init.
    k_config.keybyte_2 = 0; // 0 for not send back !keybyte2, other send back !keybyte2. 
                       // (keybyte2 is ECU response 2nd keybyte).
    // only for fast init mode, after low/high signal, command to send.
    memset(k_config.enter_cmd, 0, sizeof(k_config.enter_cmd));
}

/*********************************************************************//**
 * @brief 		K Line Begin settings
 * @param[in] 	None
 * @return 		None
 ***********************************************************************/
void KBeginSettings(void)
{
    // Disable Interrupt for UART1 channel
    NVIC_DisableIRQ(UART1_IRQn);
    // Disable UART1 line status interrupt
    UART_IntConfig((LPC_UART_TypeDef*)LPC_UART1, UART_INTCFG_RLS, DISABLE);
    // Disable UART1 Rx interrupt
    UART_IntConfig((LPC_UART_TypeDef*)LPC_UART1, UART_INTCFG_RBR, DISABLE);
    // Disable UART1 Transmit
    UART_TxCmd((LPC_UART_TypeDef*)LPC_UART1, DISABLE);
}

/*********************************************************************//**
 * @brief		write K settings to register, you can set all settings
 *              at once, and then call KWriteSettings.
 * @param[in]	None
 * @return 		None
 ***********************************************************************/
void KEndSettings(void)
{
    uart_config_struct.Baud_rate = k_config.baudrate;
    
    switch (k_config.databits) {
        case 5: uart_config_struct.Databits = UART_DATABIT_5;
        case 6: uart_config_struct.Databits = UART_DATABIT_6;
        case 7: uart_config_struct.Databits = UART_DATABIT_7;
        case 8: uart_config_struct.Databits = UART_DATABIT_8;
    }
    
    
    switch (k_config.flow_control) {
        case K_FLOWCONTROL_NONE: break;
        case K_FLOWCONTROL_HARDWARE: break;
        case K_FLOWCONTROL_SOFTWARE: break;
    }

    switch (k_config.parity) {
        case K_PARITY_NONE: uart_config_struct.Parity = UART_PARITY_NONE; break;
        case K_PARITY_ODD : uart_config_struct.Parity = UART_PARITY_ODD;  break;
        case K_PARITY_EVEN: uart_config_struct.Parity = UART_PARITY_EVEN; break;
    }
    
    switch (k_config.stopbits) {
        case K_STOPBITS_ONE: uart_config_struct.Stopbits = UART_STOPBIT_1; break;
        case K_STOPBITS_TWO: uart_config_struct.Stopbits = UART_STOPBIT_2; break;
    }

    UART_Init((LPC_UART_TypeDef*)LPC_UART1, &uart_config_struct);

	// Enable UART Transmit
	UART_TxCmd((LPC_UART_TypeDef*)LPC_UART1, ENABLE);
    
	// Enable UART Rx interrupt
	UART_IntConfig((LPC_UART_TypeDef*)LPC_UART1, UART_INTCFG_RBR, ENABLE);
    
    // Enable UART line status interrupt
    UART_IntConfig((LPC_UART_TypeDef*)LPC_UART1, UART_INTCFG_RLS, ENABLE);
    
	// Enable Interrupt for UART1 channel
	NVIC_EnableIRQ(UART1_IRQn);
    
	// preemption = 1, sub-priority = 1
	NVIC_SetPriority(UART1_IRQn, ((0x01 << 3) | 0x01));
}

/*********************************************************************//**
 * @brief		Send K Line data
 * @param[in]	data    data to be sent.
 * @param[in]   length  data length.
 * @return 		None
 ***********************************************************************/
void KSendData(uint8_t *data, size_t length)
{
    size_t i;
    for (i = 0; i < length; ++i) {
        KSendByte(data[i]);
    }
}

/*********************************************************************//**
 * @brief		K Line Send Byte
 * @param[in]	data    a byte data.
 * @return 		None
 ***********************************************************************/
void KSendByte(uint8_t data)
{
    LPC_UART1->THR = data & UART_THR_MASKBIT;
}

/*********************************************************************//**
 * @brief       K 5 baud Tx pin set
 * @param[in]   None
 * @return      None
 *********************************************************************/
static void K5BaudTxPinSet(void)
{
    // Configure USART1 Tx (P0.15) as out PP
	pin_cfg.Funcnum = PINSEL_FUNC_1;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
	pin_cfg.Pinnum = PINSEL_PIN_15;
	pin_cfg.Portnum = PINSEL_PORT_0;
	PINSEL_ConfigPin(&pin_cfg);
}

/*********************************************************************//**
 * @brief       K 5 baud start byte
 * @param[in]   None
 * @return      None
 *********************************************************************/
static void K5BaudStartByte(void)
{
    if (k_config.l_line != 0)
        GPIO_ClearValue(0, LLINE_PIN);
    GPIO_ClearValue(0, KLINE_PIN_Tx);
}

/*********************************************************************//**
 * @brief       K 5 baud send bit
 * @param[in]   mask bit
 * @return      None
 *********************************************************************/
static void K5BaudSendBit(uint8_t mask)
{
    if (k_config.addr_code & mask) {
        if (k_config.l_line != 0) GPIO_SetValue(0, LLINE_PIN);
        GPIO_SetValue(0, KLINE_PIN_Tx);
    } else {
        if (k_config.l_line != 0) GPIO_ClearValue(0, LLINE_PIN);
        GPIO_ClearValue(0, KLINE_PIN_Tx);
    }
}

/*********************************************************************//**
 * @brief       K 5 baud send address code
 * @param[in]   None
 * @return      None
 *********************************************************************/
static void K5BaudSendAddressCode(void)
{
    int i;
    for (i = 0; i < 8; i++) {
        OSTimeDlyHMSM(0, 0, 0, 200);
        K5BaudSendBit(0x01 << i);
    }
    // stop bit
    OSTimeDlyHMSM(0, 0, 0, 200);
    GPIO_SetValue(0, LLINE_PIN);
    GPIO_SetValue(0, KLINE_PIN_Tx);
    OSTimeDlyHMSM(0, 0, 0, 200);
}

/*********************************************************************//**
 * @brief       K 5 baud start auto baudrate
 * @param[in]   None
 * @return      None
 *********************************************************************/
static void K5BaudStartAutoBaud(void)
{
    k_config.bit_time = 0; // reset bit time for 5 baud init.
    k_config.bit_count = 0; // reset bit count for 5 baud init.
    exti_config_struct.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
    exti_config_struct.EXTI_polarity = EXTI_POLARITY_HIGH_ACTIVE_OR_RISING_EDGE;
    EXTI_Config(&exti_config_struct);
    
    // configure & enable Timer
    // Initialize timer 0, prescale count time of 1 uS.
    tim_config_struct.PrescaleOption = TIM_PRESCALE_USVAL;
    tim_config_struct.PrescaleValue = 1;
    
    // use channel 0, CAPn.0
    tim_capture_config_struct.CaptureChannel = 0;
    // Enable capture on CAPn.0 rising edge
    tim_capture_config_struct.RisingEdge = ENABLE;
    // Enable capture on CAPn.0 falling edge
    tim_capture_config_struct.FallingEdge = DISABLE;
    // Generate capture interrupt
    tim_capture_config_struct.IntOnCaption = ENABLE;
    
    // set configuration for Tim_config and Tim_MatchConfig
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tim_config_struct);
    TIM_ConfigCapture(LPC_TIM0, &tim_capture_config_struct);
    TIM_ResetCounter(LPC_TIM0);
    
    // preemption = 1, sub-priority = 1
    NVIC_SetPriority(TIMER0_IRQn, ((0x01 << 3) | 0x01));
    NVIC_SetPriority(EINT1_IRQn, ((0x01 << 3) | 0x01));
    
    // Enable interrupt for EXTI 1
    NVIC_EnableIRQ(EINT1_IRQn);
    
    // To start timer 0
    TIM_Cmd(LPC_TIM0, ENABLE);
}

/*********************************************************************//**
 * @brief       K 5 baud wait auto baudrate finish
 * @param[in]   None
 * @return      CLIENT_ERR_NONE for success, CLIENT_ERR_OBD_START_FAIL fail.
 *********************************************************************/
static uint8_t K5BaudWaitAutoBaudFinish(void)
{
    INT8U err = OS_ERR_NONE;
    OSMboxPend(k_config.baudrate_event, 300, &err);
    if (err == OS_ERR_NONE) {
        // set uart1 baudrate;
        k_config.baudrate = 8000000 / k_config.bit_time;
        return CLIENT_ERR_NONE;
    }
    return CLIENT_ERR_OBD_START_FAIL;
}

/*********************************************************************//**
 * @brief       K 5 baud wait key byte1 and key byte2
 * @param[out]  key byte 2
 * @return      CLIENT_ERR_NONE for success, CLIENT_ERR_OBD_START_FAIL fail.
 *********************************************************************/
static uint8_t K5BaudWaitKeyBytes(uint8_t *keybyte2)
{
    INT8U err;
    
    OSMboxPend(client_res.event, 25, &err);
    
    if (err == OS_ERR_NONE) {
        if (RingBufferPop(&client_res.buffer, keybyte2) == -1) {
            return CLIENT_ERR_OBD_START_FAIL;
        }
        
        if (RingBufferPop(&client_res.buffer, keybyte2) == -1 ) {
            // may be we need another timeout.
            OSMboxPend(client_res.event, 25, &err);
            if ((err == OS_ERR_NONE) && 
                (RingBufferPop(&client_res.buffer, keybyte2) == 0)) {
                return CLIENT_ERR_NONE;
            }
        }
    }
    return CLIENT_ERR_OBD_START_FAIL;
}

/*********************************************************************//**
 * @brief       K 5 baud send back keybyte2 reverse
 * @param[in]   Key byte2
 * @return      None
 *********************************************************************/
static void K5BaudSendKeyByte2Reverse(uint8_t keybyte2)
{
    KSendByte (~keybyte2);
}

/********************************************************************//**
 * @brief       K 5 baud receive address code reverse
 * @param[in]   None
 * @return      CLIENT_ERR_NONE success, CLIENT_ERR_OBD_START_FAIL fail.
 *********************************************************************/
static uint8_t K5BaudReceiveAddress(void)
{
    INT8U err;
    uint8_t b;
    
    OSMboxPend(client_res.event, 100, &err);
    if (err == OS_ERR_NONE) {
        if ((RingBufferPop(&client_res.buffer, &b) == 0) &&
            (b == ~k_config.addr_code)) {
            return CLIENT_ERR_NONE;
        }
    }
    return CLIENT_ERR_OBD_START_FAIL;
}

/*********************************************************************//**
 * @brief       K 5 baud synchronization
 * @param[in]   None
 * @return      CLIENT_ERR_NONE for succes, CLIENT_ERR_OBD_START_FAIL fail.
 *********************************************************************/
static uint8_t K5BaudSync(void)
{
    uint8_t err;
    uint8_t b = 0;
    KBeginSettings(); // disable all interrupt for UART1
    
    // step 1: send bps address
    K5BaudTxPinSet();
    K5BaudStartByte();
    K5BaudSendAddressCode();
    
    // step 2: wait for the synchronization pattern "0x55" before timeout.
    K5BaudStartAutoBaud();
    err = K5BaudWaitAutoBaudFinish();
    
    RingBufferReset(&client_res.buffer);
    ClientResponseRun(false); // we need to receive key bytes
    
    KEndSettings();
    
    if (err != CLIENT_ERR_NONE) {
        ClientResponseRun(true);
        return err;
    }
    
    // step 3: receive key byte 1 and key byte 2.
    if (k_config.keybyte_2) {
        err = K5BaudWaitKeyBytes(&b);
    }
    
    if (err != CLIENT_ERR_NONE) {
        ClientResponseRun(true);
        return err;
    }
    
    // step 4: send back key byte2 reverse.
    K5BaudSendKeyByte2Reverse(b);
    
    // step 5: receive address reverse before timeout.
    err = K5BaudReceiveAddress();
    
    ClientResponseRun(true);
    
    return err;
}

/*********************************************************************//**
 * @brief       K Line Initialize for begin communication
 * @param[in]   type:   K Line initialize type, fast init, or 5 baud init
 * @param[in]   lline:  If ECU Need L line to initialize?
 * @return      CLIENT_ERR_NONE for success, CLIENT_ERR_OBD_START_FAIL fail.
 *********************************************************************/
uint8_t KCommStart(KStartType type)
{
    switch (type) {
        case K_START_NONE: return CLIENT_ERR_OBD_START_FAIL;
        case K_5BAUD_SYNC: return K5BaudSync();
        case K_FAST_INIT: break;
        default: break;
    }
    
    return CLIENT_ERR_OBD_START_FAIL;
}

