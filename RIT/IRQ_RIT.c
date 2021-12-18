/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../timer/timer.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

#define N 4

extern int ricerca_massimo_negativo(int* VETT, unsigned int n);
extern int led_state;

int key2 = 0;
int key1 = 0;
int int0 = 0;

int start = 0;
int stop = 0;
int reset = 0;

void RIT_IRQHandler (void)
{					
	
	/* button management */
	if(int0 > 1){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* INT0 pressed */				
			switch(int0){
				case 2:
						LED_On(0);
						if(reset == 1 && start == 0)
						{
								reset = 0;
						}
						break;
				default:
						break;
			}
			int0++;
		}
		else {	/* button released */
			int0=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
			disable_RIT();
		}
	}
	else {
		if (int0 == 1)
			int0++;
	}
	
	/* button management */
	if(key1 > 1){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */				
			switch(key1){
				case 2:
						LED_On(1);
						if(reset != 1)
						{
							start = 1;
						}
					break;
				default:
					break;
			}
			key1++;
		}
		else {	/* button released */
			key1=0;	
			if(!start)
			{
				NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
				LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
			}
			disable_RIT();
		}
	}
	else {
		if(key1 == 1)
			key1++;
	}
	
	/* button management */
	if(key2 > 1){ 
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* INT0 pressed */				
			switch(key2){
				case 2:
					if (start == 1)
					{
						switch(stop)
						{
							case 0: 
								stop = 1;
								NVIC_DisableIRQ(EINT0_IRQn);		/* disable Button interrupts			 */
								LPC_PINCON->PINSEL4    &= ~(1 << 20);     /* GPIO pin selection */
								LED_On(2);
								LED_Off(1);
								break;
							case 1:
								stop = 0;
								NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
								LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt int0 pin selection */
								LED_Off(2);
								LED_On(1);
								break;
							default:
								break;
						}
					}
					break;
				default:
					break;
			}
			key2++;
		}
		else {	/* button released */
			key2=0;	
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
			disable_RIT();
		}
	}
	else {
		if (key2 == 1)
			key2++;
	}
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
