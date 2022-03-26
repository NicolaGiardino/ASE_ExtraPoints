/**
  ******************************************************************************
	* @author  Nicola di Gruttola Giardino 
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
	* @version v0.0.1
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 PoliTo.
  *
  *
  ******************************************************************************
  */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H__
#define __CAN_H__

/* Includes ----------------------------------------------------------------- */
#include "LPC17xx.h"
extern uint32_t SystemFrequency;
/* Defines ------------------------------------------------------------------ */

/* Error definitions */
#define CAN_OK			0x00
#define CAN_ERR_NOIRC	0x01
#define CAN_ERR_BAUD	0x02
#define CAN_ERR_BUS		0x03
#define CAN_ERR_DLC		0x04

/* Clock and Baudrate definitions */

#define CAN1_BAUD_100k(CLK_CAN)	(CLK_CAN / 100000 - 1)
#define CAN1_BAUD_125k(CLK_CAN)	(CLK_CAN / 125000 - 1)
#define CAN1_BAUD_250k(CLK_CAN)	(CLK_CAN / 250000 - 1)
#define CAN1_BAUD_1M(CLK_CAN)	(CLK_CAN / 1000000 - 1)	

/* Function prototypes -------------------------------------------------------*/

uint32_t Clk_Can1(uint32_t PCLK_CAN1) { 			
			if(PCLK_CAN1 == 0) return (SystemFrequency / 4);		
			else if(PCLK_CAN1 == 0x01) return SystemFrequency;	
			else if(PCLK_CAN1 == 0x02) return (SystemFrequency / 2);
			else return (SystemFrequency / 6);}

int CAN1_Init(const uint32_t baudrate);

int CAN1_Transmit(const uint16_t id, const uint8_t rtr, const uint8_t dlc, const uint8_t *data);

#endif /* end __CAN_H__ */
/*****************************************************************************
**                            End Of File
******************************************************************************/
