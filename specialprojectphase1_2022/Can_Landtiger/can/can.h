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
#define CAN_ERR_STB		0x05
#define CAN_ERR_AF 		0x06

/* Clock and Baudrate definitions */

#define CAN_BAUD_100k(CLK_CAN)	(CLK_CAN / 100000 - 1)
#define CAN_BAUD_125k(CLK_CAN)	(CLK_CAN / 125000 - 1)
#define CAN_BAUD_250k(CLK_CAN)	(CLK_CAN / 250000 - 1)
#define CAN_BAUD_1M(CLK_CAN)	(CLK_CAN / 1000000 - 1)	

/* Interrupt Enable Bits */
#define RIE		0x1				/* Receiver */
#define TIE1	(0x1 << 1)		/* Transmitter Buffer 1 */
#define EIE		(0x1 << 2)		/* Error Warning */
#define DOIE	(0x1 << 3)		/* Data Overrun */
#define WUIE	(0x1 << 4)		/* Wake-Up */
#define EPIE	(0x1 << 5)		/* Error Passive */
#define ALIE	(0x1 << 6)		/* Arbitration Lost */
#define BEIE	(0x1 << 7)		/* Bus Error */
#define IDIE	(0x1 << 8)		/* ID Ready */
#define TIE2	(0x1 << 9)		/* Transmit Buffer 2 */
#define TIE3	(0x1 << 10)		/* Transmit Buffer 3 */

/* Acceptance Filter Definitions */
#define	CAN1_AF		0x0
#define CAN2_AF		0x1

#define STDID		0x1
#define STDID_grp	0x2
#define EXTID		0x3
#define EXTID_grp	0x4

/* Function prototypes -------------------------------------------------------*/

uint32_t Clk_Can(uint32_t PCLK_CAN);

int  CAN1_Init(const uint32_t baudrate, const uint8_t loopback);
int  CAN1_SetPrio(const uint8_t stb, const uint8_t prio);
void CAN1_DisablePrio(void);
int  CAN1_Transmit(const uint8_t stb, const uint32_t id, const uint8_t ff, const uint8_t rtr, const uint8_t dlc, const uint8_t *data);
int  CAN1_Receive(uint32_t *id, uint8_t *ff, uint8_t *rtr, uint8_t *dlc, uint8_t *data);
void CAN1_EnableIRQ(uint16_t reg, uint32_t priority);
void CAN1_DeInit(void);

int  CAN2_Init(const uint32_t baudrate, const uint8_t loopback);
int  CAN2_SetPrio(const uint8_t stb, const uint8_t prio);
void CAN1_DisablePrio(void);
int  CAN2_Transmit(const uint8_t stb, const uint32_t id, const uint8_t ff, const uint8_t rtr, const uint8_t dlc, const uint8_t *data);
int  CAN2_Receive(uint32_t *id, uint8_t *ff, uint8_t *rtr, uint8_t *dlc, uint8_t *data);
void CAN2_EnableIRQ(uint16_t reg, uint32_t priority);
void CAN2_DeInit(void);

void CAN_AF_On(void);
void CAN_AF_Off(void);
int  CAN_AF_Add(const uint8_t controller, uint8_t type, const uint32_t startId, const uint32_t endId);
int  CAN_AF_Remove(const uint8_t controller, uint8_t type, const uint32_t startId, const uint32_t endId);

void CAN_IRQHandler(void);
#endif /* end __CAN_H__ */
/*****************************************************************************
**                            End Of File
******************************************************************************/
