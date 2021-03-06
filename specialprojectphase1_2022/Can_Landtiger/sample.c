/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: 
 
 * Note(s): this version supports the LANDTIGER Emulator
 * Author: 	Paolo BERNARDI - PoliTO - last modified 15/12/2020
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/

                  
#include "led/led.h"
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "button_EXINT/button.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "can/can.h"


/* Led external variables from funct_led */
extern unsigned char led_value;					/* defined in lib_led								*/
#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

#define RCV 1

int i, j;
uint8_t data[8], rtr, dlc, ff;
uint32_t id;
/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) 
{
	SystemInit();  												/* System Initialization (i.e., PLL)  */
	
  LCD_Initialization();
	LCD_Clear(Black);
	/* Draw the game board */
  LED_init();                           /* LED Initialization                 */
  BUTTON_init();												/* BUTTON Initialization              */
	/* 
	 * The priotiry of the ADC is higher than the one of EINT0, 
	 * so as not to have it at a higher piority than the buttons 
	 */
	//NVIC_SetPriority(ADC_IRQn, 1);
	//init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	//enable_RIT();													/* RIT enabled												*/
	
	//LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	//LPC_SC->PCON &= ~(0x2);	
	
	//LPC_PINCON->PINSEL1 |= (1<<21);
	//LPC_PINCON->PINSEL1 &= ~(1<<20);
	//LPC_GPIO0->FIODIR |= (1<<26);
	i = CAN1_Init(250000, 0);
	
	GUI_Text(0, 0, "Can send text", White, Blue);
	
	if(i)
	{
		GUI_Text(0, 20, "Error while sending", White, Blue);
		while(1)
		;
	}
#if RCV
	i = CAN_AF_Add(0, STDID, 0x01, NULL);
	if(i)
	{
		GUI_Text(0, 20, "Error while setting AF", White, Blue);
		while(1)
		;
	}
	i = CAN_AF_Add(0, STDID_grp, 0x02, 0x04);
	if(i)
	{
		GUI_Text(0, 20, "Error while setting AF", White, Blue);
		while(1)
		;
	}
	
	CAN_AF_On();
#endif
	
	data[0] = 0x1c;
	data[1] = 0x2d;
	data[2] = 0x3d;
	data[3] = 0x4d;
	data[4] = 0x5d;
	data[5] = 0x6d;
	data[6] = 0x7d;
	data[7] = 0xfd;
	
	j = 0;
	
	while (1) 
	{ 
#if RCV
		CAN1_Receive(&id, &ff, &rtr, &dlc, data);
		GUI_Text((j + 20) % 500, 0, "Received", White, Blue);
		if(id == 1)
		{
			GUI_Text((j + 20) % 500, 50, "1", White, Blue);
		}
		if(id == 2)
		{
			GUI_Text((j + 20) % 500, 100, "2", White, Blue);
		}
		if(id == 3)
		{
			GUI_Text((j + 20) % 500, 150, "3", White, Blue);
		}
		if(id == 4)
		{
			GUI_Text((j + 20) % 500, 200, "4", White, Blue);
		}
#else
		CAN1_Transmit(1, 0, 0x01, 0, 1, data);
		for(i = 0; i < 10000; i++)
		{
			__asm__("NOP");
		}
		CAN1_Transmit(2, 0, 0x02, 0, 8, data);
		for(i = 0; i < 10000; i++)
		{
			__asm__("NOP");
		}
		CAN1_Transmit(3, 0, 0x03, 0, 5, data);
		for(i = 0; i < 10000; i++)
		{
			__asm__("NOP");
		}
		CAN1_Transmit(1, 0, 0x04, 0, 2, data);
		GUI_Text((j + 20) % 1000, 0, "Sent", White, Blue);
#endif
		j++;
		for(i = 0; i < 10000; i++)
		{
			__asm__("NOP");
		}
	}

}
