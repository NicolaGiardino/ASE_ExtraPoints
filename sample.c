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

                  
#include "MyLib/functs.h"
#include "led/led.h"
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "button_EXINT/button.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "RIT/RIT.h"


/* Led external variables from funct_led */
extern unsigned char led_value;					/* defined in lib_led								*/
#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif



extern int start, stop, reset;
extern int score, record;
extern uint16_t adc_Xposition, adc_Yposition;

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) 
{
	size_t i;
	SystemInit();  												/* System Initialization (i.e., PLL)  */
	
	
  LCD_Initialization();
	LCD_Clear(Black);
	DrawLateralLines();
	LCD_PutInt(6, MAX_Y / 2, score, White, Black);
	/* Init Paddle position */
	for(i = 0; i < 5; i++)
	{
		LCD_DrawLine(adc_Xposition, adc_Yposition - i, adc_Xposition + 40, adc_Yposition - i, Green);
	}
	InitBall();
  LED_init();                           /* LED Initialization                 */
  BUTTON_init();												/* BUTTON Initialization              */
	NVIC_SetPriority(ADC_IRQn, 3);
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	enable_RIT();													/* RIT enabled												*/
	score += 100;
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);	
	
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
  while (!start) 
	{ 
		
		__ASM("wfi");
		
  }
	
	LPC_SC->PCON &= ~(0x1);
	
	PlayGame();
	
	while(1)
	{
		
	}

}
