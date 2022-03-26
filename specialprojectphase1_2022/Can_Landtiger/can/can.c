/**
  ******************************************************************************
	* @author  Nicola di Gruttola Giardino 
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
	* @version v0.0.1
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 PoliTo.
  *
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "can.h"

int CAN1_Init(const uint32_t baudrate)
{
	uint32_t brp, pclk_can;
	/* Set pin modes */
	LPC_PINCON->PINSEL0 |= (0x01) | (0x01) << 2; /* Set RD1 and TD1 */
	
	/* CAN controller initialization through CANMOD register */
	LPC_PINCON->PINMODE0 |= (0x10) | (0x10) << 2; /* Set neither pull up nor pull down*/
	
	/* Set CAN peripheral in reset mode */
	LPC_CAN1->MOD |= 0x1; /* Reset mode - Listen Only 0 - STM 0 - TPM CAN ID - SM Wake-up - RPM dominant 0 - TM Normal op */
	LPC_CAN1->MOD &= ~(0xE);
	
	/* Clear Command Register */
	LPC_CAN1->CMR = 0x0;
	
	/* If Baud > 100k do not select IRC as clock source, if so return error	*/
	if(baudrate > 100000 && LPC_SC->CLKSRCSEL == 0x00)
	{
		return -CAN_ERR_NOIRC;
	}
	
	pclk_can = (LPC_SC->PCLKSEL0 & 0x0C000000) >> 26;
	/* As of now only the standard CAn baudrates are accepted */
	switch(baudrate)
	{
		case 100000:
			brp = CAN1_BAUD_100k(Clk_Can1(pclk_can));
			break;
		case 125000:
			brp = CAN1_BAUD_125k(Clk_Can1(pclk_can));
			break;
		case 250000:
			brp = CAN1_BAUD_250k(Clk_Can1(pclk_can));
			break;
		case 1000000:
			brp = CAN1_BAUD_1M(Clk_Can1(pclk_can));
			break;
		default:
			return -CAN_ERR_BAUD;
	}
		
	LPC_CAN1->BTR |= (0xFFFFFFC0 & brp);
	
	/* Set CAN Controller in Operating Mode */
	LPC_CAN1->MOD &= ~(0x1);
 	
	return CAN_OK;
}

int CAN1_Transmit(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data)
{
	/* Check from GSR if errors are present */
	uint32_t is_err = LPC_CAN1->GSR & ((0x1 << 7) | (0xff << 16) | (0xff << 24));
	if(is_err)
	{
		return -CAN_ERR_BUS;
	}
	if(dlc > 8)
	{
		return -CAN_ERR_DLC;
	}
	/* Check from SR if the TBS/RBS bits are zero, so if bus is transmitting or receiving */
	while(LPC_CAN1->SR & ((0x1 << 2) | (0x1 << 8) | (0x1 << 10) | (0x1 << 18)))
		;
	
	/* Set PRIO not used in this DEMO, only Transmit Buffer 1 used,
	 * 2 and 3 for a later implementation,	
	 * only DLC, RTR are set and FF is set to standard
	 */
	LPC_CAN1->TFI1 = (0x00 | ((0x0F & dlc) << 16) | ((0x01 & rtr) << 30) | (0x0 << 31));
	
	/* Set id */
	LPC_CAN1->TID1 |= (0x03FF & id);
	
	/* If not RTR, set the data to be sent */
	if(!rtr)
	{
		uint8_t i;
		for(i = 0; i < dlc && i < 4; i++)
		{
			LPC_CAN1->TDA1 |= (data[i] << i*8);
		}
		for(i = 4; i < dlc; i++)
		{
			LPC_CAN1->TDA1 |= (data[i] << (i - 4)*8);
		}
	}
	
	/* Set command to send message (TR and SB1) */
	LPC_CAN1->CMR |= (0x1 | (0x1 << 5));
	
	/* Check from SR if the TBS bits are zero, so if bus is still transmitting */
	while(LPC_CAN1->SR & ((0x1 << 2) | (0x1 << 10) | (0x1 << 18)))
		;
	
	return CAN_OK;
	
}




/*****************************************************************************
**                            End Of File
******************************************************************************/
