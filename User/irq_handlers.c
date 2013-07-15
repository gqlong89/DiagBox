/**********************************************************************
* $Id$		irq_handlers.c				2013-07-09
*//**
* @file		irq_handlers.c
* @brief	IRQ handler source codes
* @version	1.0
* @date		09. July. 2013
* @author	Rocky Weshare Xu
*
* Copyright(C) 2013, DNT Diagnostic And Tools
* All rights reserved.
*
**********************************************************************/

/* Includes ------------------------------------------------------------------- */
#include "includes.h"

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/

/*********************************************************************//**
 * @brief		UART Line Status Error
 * @param[in]	err_type:   UART Line Status Error Type
 * @return 		None
 **********************************************************************/
static void UartIntErr(uint8_t err_type)
{
	uint8_t test;
	test = test;
 	// Loop forever
 	for (;;) {
 		test = err_type;
 	}
 }

/*********************************************************************//**
 * @brief		UART0 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void UART0_IRQHandler(void)
{
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
        uint8_t tmpc;
        uint32_t len;
        for (;;) {
            // Call UART read function in UART driver 
            len = UART_Receive((LPC_UART_TypeDef*)LPC_UART0, &tmpc, 1, NONE_BLOCKING);
            // If data receive
            if (len) {
                RingBufferPush(&client_parser.buffer, tmpc);
            } else {
                // No more data
                OSMboxPost(client_parser.event, (void*)1);
                break;
            }
        }
	}

	// Transmit Holding Empty
	if (tmp == UART_IIR_INTID_THRE) {

	}
}

/*********************************************************************//**
 * @brief		UART1 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void UART1_IRQHandler(void)
{
    int32_t intsrc, tmp, tmp1;
    
    // Determine the interrupt source
    intsrc = UART_GetIntId((LPC_UART_TypeDef*)LPC_UART1);
    tmp = intsrc & UART_IIR_INTID_MASK;
    
    // Receive Line Status
    if (tmp == UART_IIR_INTID_RLS) {
        // Check Line Status
        tmp1 = UART_GetLineStatus((LPC_UART_TypeDef*)LPC_UART1);
        // Mask out the receive ready and transmit holding empty status
        tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_BI | UART_LSR_RXFE);
        
        // If any error exist
        if (tmp1) {
            UartIntErr(tmp1);
        }
    }
    
    // Receive data available or character time-out
    if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI)) {
        uint8_t tmpc;
        uint32_t len;
        for (;;) {
            // Call UART read function in UART driver 
            len = UART_Receive((LPC_UART_TypeDef*)LPC_UART1, &tmpc, 1, NONE_BLOCKING);
            // If data receive
            if (len) {
                RingBufferPush(&client_parser.buffer, tmpc);
            } else {
                // No more data
                if (client_res.is_response)
                    OSMboxPost(client_parser.event, (void*)1);
                break;
            }
        }
    }
    
    // Tramsmit Holding Empty
    if (tmp == UART_IIR_INTID_THRE) {
    }
}

/*********************************************************************//**
 * @brief		EXTI1 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void EINT1_IRQHandler(void)
{
    EXTI_ClearEXTIFlag(EXTI_EINT1);
    k_config.bit_time += TIM_GetCaptureValue(LPC_TIM0, TIM_COUNTER_INCAP0);
    k_config.bit_count += 2;
    if (k_config.bit_count == 8) {
        TIM_Cmd(LPC_TIM0, DISABLE);
        NVIC_DisableIRQ(EINT1_IRQn);
        OSMboxPost(k_config.baudrate_event, (void *)1);
    }
}
