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
#include "../adc/adc.h"
#include "../MyLib/functs.h"

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
extern int score;

extern uint16_t adc_Xposition, adc_Yposition;

int key2 = 0;
int key1 = 0;
int int0 = 0;

int start = 0;
int stop = 0;
int reset = 0;

void RIT_IRQHandler (void)
{					
	size_t i;
	
	
	/* INT0 button management */
	if(int0 > 1)
	{ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0)
		{				
			switch(int0){
				case 2:
						/* When INT0 is pressed, after having lost a game
						 * it sets reset to zero and reactivates the IRQ of KEY1
						 */
						if(reset == 1 && start == 0)
						{
								reset = 0;
								NVIC_EnableIRQ(EINT1_IRQn);
								LPC_PINCON->PINSEL4    |= (1 << 22);
						}
						break;
				default:
						break;
			}
			int0++;
		}
		else 
		{
			int0=0;			
			NVIC_EnableIRQ(EINT0_IRQn);
			LPC_PINCON->PINSEL4    |= (1 << 20);
		}
	}
	else 
	{
		if (int0 == 1)
			int0++;
	}
	
	/* KEY1 button management */
	if(key1 > 1)
	{ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0)
		{			
			switch(key1)
			{
				case 2:
						/* If the KEY1 button is pressed at the beginning or after INT0
						 * it sets start to 1, allowing the game to be played,
						 * initiates the ADC and then... (follows in the else)
						 */
						if(reset != 1)
						{
							LCD_Clear(Black);
							score = 0;
							DrawLateralLines();
							LCD_PutInt(6, MAX_Y / 2, score, White, Black);
							/* Init Paddle position */
							for(i = 0; i < 40; i++)
							{
								LCD_DrawLine(adc_Xposition + 20 - i, adc_Yposition, adc_Xposition + 20 - i, adc_Yposition + 5, Green);
							}
							InitBall();
							start = 1;
							ADC_init();
						}
					break;
				default:
					break;
			}
			key1++;
		}
		else 
		{
			key1=0;	
			/* If the button has been truly pressed
			 * it disables the IRQ for KEY1
			 */
			if(!start)
			{
				NVIC_EnableIRQ(EINT1_IRQn);
				LPC_PINCON->PINSEL4    |= (1 << 22);
			}
		}
	}
	else {
		if(key1 == 1)
			key1++;
	}
	
	/* KEY2 button management */
	if(key2 > 1){ 
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){			
			switch(key2){
				case 2:
					/* If KEY2 is pressed while the game is playing */
					if (start == 1)
					{
						switch(stop)
						{
							/* If the game wasn't previously stopped
							 * it disables the IRQ of INT0, stops the ADC and enters the loop
							 */
							case 0: 
								stop = 1;
								NVIC_DisableIRQ(EINT0_IRQn);
								LPC_PINCON->PINSEL4    &= ~(1 << 20);
							  NVIC_DisableIRQ(ADC_IRQn);
								break;
							/* If the game was previously stopped
							 * it enables the IRQ of INT0, reactivates the ADC and exits the loop
							 */
							case 1:
								stop = 0;
								NVIC_EnableIRQ(EINT0_IRQn);
								NVIC_EnableIRQ(ADC_IRQn);
								LPC_PINCON->PINSEL4    |= (1 << 20);
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
		else 
		{
			key2=0;	
			NVIC_EnableIRQ(EINT2_IRQn);
			LPC_PINCON->PINSEL4    |= (1 << 24);
		}
	}
	else 
	{
		if (key2 == 1)
			key2++;
	}
	
	if(start == 1)
	{
		ADC_start_conversion();
	}
	else if(!start && reset == 1)
	{
		GameLost();
	}
	
	if(LPC_RIT->RICOUNTER > 0x004C4B40)
		LPC_RIT->RICOUNTER = 0;
	
  LPC_RIT->RICTRL |= 0x1;
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
