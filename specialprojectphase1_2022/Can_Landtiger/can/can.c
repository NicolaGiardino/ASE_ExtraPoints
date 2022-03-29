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
#include "lpc17xx.h"
#include "can.h"

static uint8_t loopback_can1, loopback_can2;

uint32_t Clk_Can(uint32_t PCLK_CAN) 
{ 			
	if(PCLK_CAN == 0) 
		return (SystemFrequency / 4);		
	else if(PCLK_CAN == 0x01) 
		return SystemFrequency;	
	else if(PCLK_CAN == 0x02) 
		return (SystemFrequency / 2);
	else 
		return (SystemFrequency / 6);
}


/*---------------------------------------------------------------------------
**                         Functions for CAN1
**---------------------------------------------------------------------------*/

static void CAN1_Transmit_STB1(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data);
static void CAN1_Transmit_STB2(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data);
static void CAN1_Transmit_STB3(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data);

int CAN1_Init(const uint32_t baudrate, const uint8_t loopback)
{
	uint32_t brp, pclk_can, result;
	uint8_t NT, TSEG1 = 0, TSEG2 = 0;
	/* Set pin modes */
	LPC_PINCON->PINSEL0 &= ~(0xFF | (0xFF << 2));
	LPC_PINCON->PINSEL0 |= (0x01) | (0x01 << 2); /* Set RD1 and TD1 */
	
	/* CAN controller initialization through CANMOD register */
	LPC_PINCON->PINMODE0 &= ~(0xFF | (0xFF << 2));
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
			brp = CAN_BAUD_100k(Clk_Can(pclk_can));
			break;
		case 125000:
			brp = CAN_BAUD_125k(Clk_Can(pclk_can));
			break;
		case 250000:
			brp = CAN_BAUD_250k(Clk_Can(pclk_can));
			break;
		case 1000000:
			brp = CAN_BAUD_1M(Clk_Can(pclk_can));
			break;
		default:
			return -CAN_ERR_BAUD;
	}
	result = Clk_Can(pclk_can) / baudrate;

	/* Calculate suitable nominal time value
	 * NT (nominal time) = (TSEG1 + TSEG2 + 3)
	 * NT <= 24
	 * TSEG1 >= 2*TSEG2
	 */
	for (NT = 24; NT > 0; NT = NT - 2) {
		if ((result % NT) == 0) {
			brp = result / NT - 1;

			NT--;

			TSEG2 = (NT / 3) - 1;

			TSEG1 = NT - (NT / 3) - 1;

			break;
		}
	}
	if (NT == 0) {
		return -CAN_ERR_BAUD;
	}
		
	LPC_CAN1->BTR = ((0x7 & TSEG2) << 20) | ((0xF & TSEG1) << 16) | (3 << 14) | (0x000003FF & brp);
	
	/* This for  LoopBack*/
	if(loopback)
	{
		LPC_CAN1->MOD |= (0x1 << 2);
	}
	loopback_can1 = loopback;
	/* Set CAN Controller in Operating Mode */
	LPC_CAN1->MOD &= ~(0x1);
 	
	return CAN_OK;
}



static void CAN1_Transmit_STB1(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data)
{
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
			LPC_CAN1->TDB1 |= (data[i] << (i - 4)*8);
		}
	}
	
	/* Set command to send message (TR and SB1) */
	if(loopback_can1)
	{
		LPC_CAN1->CMR |= ((0x1 << 4) | (0x1 << 5));
	}
	else
	{
		LPC_CAN1->CMR |= ((0x1) | (0x1 << 5));
	}
}

static void CAN1_Transmit_STB2(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data)
{
	/* Set PRIO not used in this DEMO, only Transmit Buffer 1 used,
	 * 2 and 3 for a later implementation,	
	 * only DLC, RTR are set and FF is set to standard
	 */
	LPC_CAN1->TFI2 = (0x00 | ((0x0F & dlc) << 16) | ((0x01 & rtr) << 30) | (0x0 << 31));
	
	/* Set id */
	LPC_CAN1->TID2 |= (0x03FF & id);
	
	/* If not RTR, set the data to be sent */
	if(!rtr)
	{
		uint8_t i;
		for(i = 0; i < dlc && i < 4; i++)
		{
			LPC_CAN1->TDA2 |= (data[i] << i*8);
		}
		for(i = 4; i < dlc; i++)
		{
			LPC_CAN1->TDB2 |= (data[i] << (i - 4)*8);
		}
	}
	
	/* Set command to send message (TR and SB1) */
	if(loopback_can1)
	{
		LPC_CAN1->CMR |= ((0x1 << 4) | (0x1 << 6));
	}
	else
	{
		LPC_CAN1->CMR |= ((0x1) | (0x1 << 6));
	}
}

static void CAN1_Transmit_STB3(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data)
{
	/* Set PRIO not used in this DEMO, only Transmit Buffer 1 used,
	 * 2 and 3 for a later implementation,	
	 * only DLC, RTR are set and FF is set to standard
	 */
	LPC_CAN1->TFI3 = (0x00 | ((0x0F & dlc) << 16) | ((0x01 & rtr) << 30) | (0x0 << 31));
	
	/* Set id */
	LPC_CAN1->TID3 |= (0x03FF & id);
	
	/* If not RTR, set the data to be sent */
	if(!rtr)
	{
		uint8_t i;
		for(i = 0; i < dlc && i < 4; i++)
		{
			LPC_CAN1->TDA3 |= (data[i] << i*8);
		}
		for(i = 4; i < dlc; i++)
		{
			LPC_CAN1->TDB3 |= (data[i] << (i - 4)*8);
		}
	}
	
	/* Set command to send message (TR and SB1) */
	if(loopback_can1)
	{
		LPC_CAN1->CMR |= ((0x1 << 4) | (0x1 << 7));
	}
	else
	{
		LPC_CAN1->CMR |= ((0x1) | (0x1 << 7));
	}
}

int CAN1_Transmit(const uint8_t stb, const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data)
{
	/* Check from GSR if errors are present */
	uint32_t is_err;
	
	is_err	= LPC_CAN1->GSR & ((0x1 << 7) | (0xff << 16) | (0xff << 24));
	if(is_err)
	{
		return -CAN_ERR_BUS;
	}
	if(dlc > 8)
	{
		return -CAN_ERR_DLC;
	}
	/* Check from SR if the TBS/RBS bits are zero, so if bus is transmitting or receiving */
	while((LPC_CAN1->SR & ((0x1 << 2) | (0x1 << 10) | (0x1 << 18))) != ((0x1 << 2) | (0x1 << 10) | (0x1 << 18)))
		;
	
	switch(stb)
	{
		case 1:
			CAN1_Transmit_STB1(id, rtr, dlc, data);
			break;
		case 2:
			CAN1_Transmit_STB2(id, rtr, dlc, data);
			break;
		case 3:
			CAN1_Transmit_STB3(id, rtr, dlc, data);
			break;
		default:
			return -CAN_ERR_STB;
	}
	
	/* Check from SR if the TBS bits are zero, so if bus is still transmitting */
	while((LPC_CAN1->SR & (0x1 << 2)) != (0x1 << 2))
		;
	
	return CAN_OK;
	
}

int CAN1_Receive(uint16_t *id, uint8_t *rtr, uint8_t *dlc, uint8_t *data)
{
	/* Check from GSR if errors are present */
	uint32_t is_err = LPC_CAN1->GSR & ((0x1 << 7) | (0xff << 16) | (0xff << 24));
	if(is_err)
	{
		return -CAN_ERR_BUS;
	}
	
	/* Wait for the data to be received */
	while(!(LPC_CAN1->GSR & ((0x1) | (0x1 << 4))))
		;
	
	/* Set id, rtr and data lenght */
	*id = LPC_CAN1->RID & 0x7FF;
	*rtr = (LPC_CAN1->RFS >> 30) & 0x1;
	*dlc = (LPC_CAN1->RFS >> 16) & 0xF;
	
	/* If not RTR, set the data received */
	if(!(*rtr))
	{
		uint8_t i;
		for(i = 0; i < *dlc && i < 4; i++)
		{
			 data[i] = (uint8_t)(LPC_CAN1->RDA << i*8);
		}
		for(i = 4; i < *dlc; i++)
		{
			data[i] = (uint8_t)(LPC_CAN1->RDB << (i - 4)*8);
		}
	}
	
	/* Once the message has been received, release the bus by setting the RRB bit */
	LPC_CAN1->CMR |= (0x1 << 2);
	
	return CAN_OK;
	
}

void CAN1_EnableIRQ(uint16_t reg, uint32_t priority)
{
	LPC_CAN1->IER = reg & 0x7FF;
	
	NVIC_EnableIRQ(CAN_IRQn);              /* enable irq in nvic                 */
	NVIC_SetPriority(CAN_IRQn, priority);	   /* priority, the lower the better     */
}

/*---------------------------------------------------------------------------
**                         End functions for CAN1
**---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
**                         Functions for CAN2
**---------------------------------------------------------------------------*/


static void CAN2_Transmit_STB1(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data);
static void CAN2_Transmit_STB2(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data);
static void CAN2_Transmit_STB3(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data);

int CAN2_Init(const uint32_t baudrate, const uint8_t loopback)
{
	uint32_t brp, pclk_can, result;
	uint8_t NT, TSEG1 = 0, TSEG2 = 0;
	/* Set pin modes */
	LPC_PINCON->PINSEL4 &= ~((0xFF << 14) | (0xFF << 16));
	LPC_PINCON->PINSEL4 |= (0x01 << 14) | (0x01 << 16); /* Set RD2 and TD2 */
	
	/* CAN controller initialization through CANMOD register */
	LPC_PINCON->PINMODE4 &= ~((0xFF << 14) | (0xFF << 16));
	LPC_PINCON->PINMODE4 |= (0x10 << 14) | (0x10 << 16); /* Set neither pull up nor pull down*/
	
	/* Set CAN peripheral in reset mode */
	LPC_CAN2->MOD |= 0x1; /* Reset mode - Listen Only 0 - STM 0 - TPM CAN ID - SM Wake-up - RPM dominant 0 - TM Normal op */
	LPC_CAN2->MOD &= ~(0xE);
	
	/* Clear Command Register */
	LPC_CAN2->CMR = 0x0;
	
	/* If Baud > 100k do not select IRC as clock source, if so return error	*/
	if(baudrate > 100000 && LPC_SC->CLKSRCSEL == 0x00)
	{
		return -CAN_ERR_NOIRC;
	}
	
	pclk_can = (LPC_SC->PCLKSEL0 << 28) & 0x3;
	/* As of now only the standard CAn baudrates are accepted */
	switch(baudrate)
	{
		case 100000:
			brp = CAN_BAUD_100k(Clk_Can(pclk_can));
			break;
		case 125000:
			brp = CAN_BAUD_125k(Clk_Can(pclk_can));
			break;
		case 250000:
			brp = CAN_BAUD_250k(Clk_Can(pclk_can));
			break;
		case 1000000:
			brp = CAN_BAUD_1M(Clk_Can(pclk_can));
			break;
		default:
			return -CAN_ERR_BAUD;
	}
	result = Clk_Can(pclk_can) / baudrate;

	/* Calculate suitable nominal time value
	 * NT (nominal time) = (TSEG1 + TSEG2 + 3)
	 * NT <= 24
	 * TSEG1 >= 2*TSEG2
	 */
	for (NT = 24; NT > 0; NT = NT - 2) {
		if ((result % NT) == 0) {
			brp = result / NT - 1;

			NT--;

			TSEG2 = (NT / 3) - 1;

			TSEG1 = NT - (NT / 3) - 1;

			break;
		}
	}
	if (NT == 0) {
		return -CAN_ERR_BAUD;
	}
		
	LPC_CAN2->BTR = ((0x7 & TSEG2) << 20) | ((0xF & TSEG1) << 16) | (3 << 14) | (0x000003FF & brp);
	
	/* This for  LoopBack*/
	if(loopback)
	{
		LPC_CAN2->MOD |= (0x1 << 2);
	}
	loopback_can2 = loopback;
	/* Set CAN Controller in Operating Mode */
	LPC_CAN2->MOD &= ~(0x1);
 	
	return CAN_OK;
}



static void CAN2_Transmit_STB1(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data)
{
	/* Set PRIO not used in this DEMO, only Transmit Buffer 1 used,
	 * 2 and 3 for a later implementation,	
	 * only DLC, RTR are set and FF is set to standard
	 */
	LPC_CAN2->TFI1 = (0x00 | ((0x0F & dlc) << 16) | ((0x01 & rtr) << 30) | (0x0 << 31));
	
	/* Set id */
	LPC_CAN2->TID1 |= (0x03FF & id);
	
	/* If not RTR, set the data to be sent */
	if(!rtr)
	{
		uint8_t i;
		for(i = 0; i < dlc && i < 4; i++)
		{
			LPC_CAN2->TDA1 |= (data[i] << i*8);
		}
		for(i = 4; i < dlc; i++)
		{
			LPC_CAN2->TDB1 |= (data[i] << (i - 4)*8);
		}
	}
	
	/* Set command to send message (TR and SB1) */
	if(loopback_can2)
	{
		LPC_CAN2->CMR |= ((0x1 << 4) | (0x1 << 5));
	}
	else
	{
		LPC_CAN2->CMR |= ((0x1) | (0x1 << 5));
	}
}

static void CAN2_Transmit_STB2(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data)
{
	/* Set PRIO not used in this DEMO, only Transmit Buffer 1 used,
	 * 2 and 3 for a later implementation,	
	 * only DLC, RTR are set and FF is set to standard
	 */
	LPC_CAN2->TFI2 = (0x00 | ((0x0F & dlc) << 16) | ((0x01 & rtr) << 30) | (0x0 << 31));
	
	/* Set id */
	LPC_CAN2->TID2 |= (0x03FF & id);
	
	/* If not RTR, set the data to be sent */
	if(!rtr)
	{
		uint8_t i;
		for(i = 0; i < dlc && i < 4; i++)
		{
			LPC_CAN2->TDA2 |= (data[i] << i*8);
		}
		for(i = 4; i < dlc; i++)
		{
			LPC_CAN2->TDB2 |= (data[i] << (i - 4)*8);
		}
	}
	
	/* Set command to send message (TR and SB1) */
	if(loopback_can2)
	{
		LPC_CAN2->CMR |= ((0x1 << 4) | (0x1 << 6));
	}
	else
	{
		LPC_CAN2->CMR |= ((0x1) | (0x1 << 6));
	}
}

static void CAN2_Transmit_STB3(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data)
{
	/* Set PRIO not used in this DEMO, only Transmit Buffer 1 used,
	 * 2 and 3 for a later implementation,	
	 * only DLC, RTR are set and FF is set to standard
	 */
	LPC_CAN2->TFI3 = (0x00 | ((0x0F & dlc) << 16) | ((0x01 & rtr) << 30) | (0x0 << 31));
	
	/* Set id */
	LPC_CAN2->TID3 |= (0x03FF & id);
	
	/* If not RTR, set the data to be sent */
	if(!rtr)
	{
		uint8_t i;
		for(i = 0; i < dlc && i < 4; i++)
		{
			LPC_CAN2->TDA3 |= (data[i] << i*8);
		}
		for(i = 4; i < dlc; i++)
		{
			LPC_CAN2->TDB3 |= (data[i] << (i - 4)*8);
		}
	}
	
	/* Set command to send message (TR and SB1) */
	if(loopback_can2)
	{
		LPC_CAN2->CMR |= ((0x1 << 4) | (0x1 << 7));
	}
	else
	{
		LPC_CAN2->CMR |= ((0x1) | (0x1 << 7));
	}
}

int CAN2_Transmit(const uint8_t stb, const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data)
{
	/* Check from GSR if errors are present */
	uint32_t is_err;
	
	is_err	= LPC_CAN2->GSR & ((0x1 << 7) | (0xff << 16) | (0xff << 24));
	if(is_err)
	{
		return -CAN_ERR_BUS;
	}
	if(dlc > 8)
	{
		return -CAN_ERR_DLC;
	}
	/* Check from SR if the TBS/RBS bits are zero, so if bus is transmitting or receiving */
	while((LPC_CAN2->SR & ((0x1 << 2) | (0x1 << 10) | (0x1 << 18))) != ((0x1 << 2) | (0x1 << 10) | (0x1 << 18)))
		;
	
	switch(stb)
	{
		case 1:
			CAN2_Transmit_STB1(id, rtr, dlc, data);
			break;
		case 2:
			CAN2_Transmit_STB2(id, rtr, dlc, data);
			break;
		case 3:
			CAN2_Transmit_STB3(id, rtr, dlc, data);
			break;
		default:
			return -CAN_ERR_STB;
	}
	
	/* Check from SR if the TBS bits are zero, so if bus is still transmitting */
	while((LPC_CAN2->SR & (0x1 << 2)) != (0x1 << 2))
		;
	
	return CAN_OK;
	
}

int CAN2_Receive(uint16_t *id, uint8_t *rtr, uint8_t *dlc, uint8_t *data)
{
	/* Check from GSR if errors are present */
	uint32_t is_err = LPC_CAN2->GSR & ((0x1 << 7) | (0xff << 16) | (0xff << 24));
	if(is_err)
	{
		return -CAN_ERR_BUS;
	}
	
	/* Wait for the data to be received */
	while(!(LPC_CAN2->GSR & ((0x1) | (0x1 << 4))))
		;
	
	/* Set id, rtr and data lenght */
	*id = LPC_CAN2->RID & 0x7FF;
	*rtr = (LPC_CAN2->RFS >> 30) & 0x1;
	*dlc = (LPC_CAN2->RFS >> 16) & 0xF;
	
	/* If not RTR, set the data received */
	if(!(*rtr))
	{
		uint8_t i;
		for(i = 0; i < *dlc && i < 4; i++)
		{
			 data[i] = (uint8_t)(LPC_CAN2->RDA << i*8);
		}
		for(i = 4; i < *dlc; i++)
		{
			data[i] = (uint8_t)(LPC_CAN2->RDB << (i - 4)*8);
		}
	}
	
	/* Once the message has been received, release the bus by setting the RRB bit */
	LPC_CAN2->CMR |= (0x1 << 2);
	
	return CAN_OK;
	
}

void CAN2_EnableIRQ(uint16_t reg, uint32_t priority)
{
	LPC_CAN2->IER = reg & 0x7FF;
	
	NVIC_EnableIRQ(CAN_IRQn);              /* enable irq in nvic                 */
	NVIC_SetPriority(CAN_IRQn, priority);	   /* priority, the lower the better     */
}

/*---------------------------------------------------------------------------
**                         End functions for CAN2
**---------------------------------------------------------------------------*/

/*****************************************************************************
**                            End Of File
******************************************************************************/
