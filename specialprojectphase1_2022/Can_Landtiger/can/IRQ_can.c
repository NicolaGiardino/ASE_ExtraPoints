#include "lpc17xx.h"
#include "can.h"

uint32_t can1_icr, can2_icr, can1_ier, can2_ier;

void CAN1_IRQHandler(void)
{
	/* Disable and clear all interrupts */
	
	NVIC_DisableIRQ(CAN_IRQn);
	
	can1_ier = LPC_CAN1->IER;
	can1_icr = LPC_CAN1->ICR;
	LPC_CAN1->IER = 0;
	
	can2_ier = LPC_CAN2->IER;
	can2_icr = LPC_CAN2->ICR;
	LPC_CAN2->IER = 0;
	
	/* Do your things */
	
	
	
	/* Enable the interrupts */
	LPC_CAN1->IER = can1_ier & 0x7FF;
	LPC_CAN2->IER = can2_ier & 0x7FF;
	
	NVIC_EnableIRQ(CAN_IRQn);
}
