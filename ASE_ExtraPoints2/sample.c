/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: 
 *		to control led11 and led 10 through EINT buttons (similarly to project 03_)
 *		to control leds9 to led4 by the timer handler (1 second - circular cycling)
 * Note(s): this version supports the LANDTIGER Emulator
 * Author: 	Paolo BERNARDI - PoliTO - last modified 15/12/2020
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/

/******************************************************************************
 * This version of the code implements a multiplayer version of the EP1				*
 *																																						*
 * Some improvements to the code have been made																*
 * as well as many bug fixes.																									*
 *																																						*
 * Nicola di Gruttola Giardino  																							*
 * Github:@NicolaGiardino																											*
 ******************************************************************************/
                  
#include "MyLib/functs.h"
#include "led/led.h"
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "button_EXINT/button.h"
#include "GLCD/GLCD.h" 
#include "GLCD/AsciiLib.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "RIT/RIT.h"


/* Led external variables from funct_led */
extern unsigned char led_value;					/* defined in lib_led								*/
#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) 
{
	SystemInit();  												/* System Initialization (i.e., PLL)  */
	
	
  LCD_Initialization();
	LCD_Clear(Black);
	/* Draw the game board */
	GUI_Text(MAX_X / 2 - 100, MAX_Y / 2, "Press KEY1 to Start", White, Black);
  LED_init();                           /* LED Initialization                 */
  BUTTON_init();												/* BUTTON Initialization              */
	/* 
	 * The priotiry of the ADC is higher than the one of EINT0, 
	 * so as not to have it at a higher piority than the buttons 
	 */
	NVIC_SetPriority(ADC_IRQn, 1);
	init_RIT(0x002625A0);									/* RIT Initialization 50 msec       	*/
	enable_RIT();													/* RIT enabled												*/
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);	
	
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
  while (1) 
	{ 
		
		__ASM("wfi");
		
  }

}
