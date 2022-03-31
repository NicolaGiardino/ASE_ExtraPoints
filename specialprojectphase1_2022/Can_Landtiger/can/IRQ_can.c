#include "lpc17xx.h"
#include "can.h"
#include "../GLCD/GLCD.h" 

uint32_t can1_icr, can2_icr, can1_ier, can2_ier;

void CAN_IRQHandler(void)
{
	/* Disable and clear all interrupts */
	
	NVIC_DisableIRQ(CAN_IRQn);
	
	can1_ier = LPC_CAN1->IER & 0x7FF;
	can1_icr = LPC_CAN1->ICR >> 16;
	LPC_CAN1->IER &= 0xfffff800;
	
	can2_ier = LPC_CAN2->IER & 0x7FF;
	can2_icr = LPC_CAN2->ICR;
	LPC_CAN2->IER &= 0xfffff800;
	
	/* Do your things */
	
	/*if (can1_icr == 0) {												// if there are no errors in the communication
		GUI_Text(0, 60, "OK", White, Blue);
	}*/
	
	/* Enable the interrupts */
	LPC_CAN1->IER |= can1_ier;
	LPC_CAN2->IER |= can2_ier;
	
	NVIC_EnableIRQ(CAN_IRQn);
}
