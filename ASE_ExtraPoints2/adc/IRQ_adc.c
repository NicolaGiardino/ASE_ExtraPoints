/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "adc.h"
#include "../led/led.h"
#include "../TouchPanel/TouchPanel.h"
#include "../GLCD/GLCD.h"
#include "../MyLib/functs.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

#define MAX_PADDLE 0xD60 /* 3424 decimal */
#define MIN_PADDLE 0x281 /* 641 decimal */

unsigned short AD_current;   
unsigned short AD_last = 0xFF;     /* Last converted value               */

uint16_t adc_Xposition = MAX_X / 2 - 20;
uint16_t adc_Yposition = MAX_Y - 31;

uint16_t bot_Xposition = 16;
uint16_t bot_Yposition = 31;

static uint16_t lastX;

/********************************************************************************
*                                                                               *
* FUNCTION NAME: MovePotentiometer                                              *
*                                                                               *
* PURPOSE: Function to move the paddle, 																				*
*						based on the current ADC position																		*
* ARGUMENT LIST:                                                                *
*                                                                               *
* Argument  Type         IO     Description                                     *
* --------- --------     --     ---------------------------------               *
*                                                                               *
* RETURN VALUE: void                                                            *
*                                                                               *
********************************************************************************/
void MovePotentiometer()
{
	size_t i;
	
	lastX = adc_Xposition;
	
	/* The paddle goes from where the potentiometer is: [-  ] to [  -], clockwise */
	if(AD_current < MIN_PADDLE)
	{
		adc_Xposition = 6;
	}
	else if(AD_current > MAX_PADDLE)
	{
		adc_Xposition = MAX_X - 46;
	}
	else
	{
		adc_Xposition = (AD_current - MIN_PADDLE) * (MAX_X - 46) / (MAX_PADDLE - MIN_PADDLE) + 6;
	}
	if((adc_Xposition - lastX) < 5 && (adc_Xposition - lastX) > -5)
	{
		adc_Xposition = lastX;
		return;
	}
	for(i = 0; i < 40; i++)
	{
		/* Clear last paddle */
		if((lastX + i) < adc_Xposition || (lastX + i) > (adc_Xposition + 39))
		{
			LCD_DrawLine(lastX + i, adc_Yposition, lastX + i, adc_Yposition + 9, Black);
		}
		/* Set new paddle */
		if((adc_Xposition + i) < lastX || (adc_Xposition + i) > (lastX + 39))
		{
			LCD_DrawLine(adc_Xposition + i, adc_Yposition, adc_Xposition + i, adc_Yposition + 9, Green);
		}
	}
	
}

/********************************************************************************
*                                                                               *
* FUNCTION NAME: MovePotentiometer                                              *
*                                                                               *
* PURPOSE: Function to move the paddle of the bot 															*
* ARGUMENT LIST:                                                                *
*                                                                               *
* Argument  Type         IO     Description                                     *
* --------- --------     --     ---------------------------------               *
*                                                                               *
* RETURN VALUE: void                                                            *
*                                                                               *
********************************************************************************/
static uint16_t prevBotX = MAX_X - 46;
static int16_t sign = 1;
void MoveBot()
{
	size_t i;
	
	bot_Xposition = sign > 0 ? bot_Xposition + 2 : (uint16_t)((int16_t)bot_Xposition - 2);
	
	if(bot_Xposition < 8)
	{
		bot_Xposition = 6;
		sign = 1;
	}
	else if(bot_Xposition > MAX_X - 48)
	{
		bot_Xposition = MAX_X - 46;
		sign = -1;
	}
	
	for(i = 0; i < 40; i++)
	{
		/* Clear last paddle */
		if((prevBotX + i) < bot_Xposition || (prevBotX + i) > (bot_Xposition + 39))
		{
			LCD_DrawLine(prevBotX + i, bot_Yposition - 9, prevBotX + i, bot_Yposition, Black);
		}
	}
	for(i = 0; i < 40; i++)
	{
		/* Set new paddle */
		if((bot_Xposition + i) < prevBotX || (bot_Xposition + i) > (prevBotX + 39))
		{
			LCD_DrawLine(bot_Xposition + i, bot_Yposition - 9, bot_Xposition + i, bot_Yposition, Green);
		}
	}
	
	prevBotX = bot_Xposition;
	
}


void ADC_IRQHandler(void) {
  	
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result */
  if(AD_current != AD_last)
	{
		AD_last = AD_current;
		MovePotentiometer();
  }	
	MoveBot();
	MoveBall();
}
