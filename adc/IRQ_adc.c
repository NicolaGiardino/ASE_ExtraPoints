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

#define MAX_PADDLE 0xD60 /* 641 decimal */
#define MIN_PADDLE 0x281 /* 3424 decimal */

unsigned short AD_current;   
unsigned short AD_last = 0xFF;     /* Last converted value               */

uint16_t adc_Xposition = MAX_X / 2 - 20;
uint16_t adc_Yposition = MAX_Y - 33;

static uint16_t lastX;
static uint16_t lastY;

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
	lastY = adc_Yposition;
	
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
	for(i = 0; i < 40; i++)
	{
		/* Clear last paddle */
		if((lastX + i) < adc_Xposition || (lastX + i) > (adc_Xposition + 40))
		{
			LCD_DrawLine(lastX + i, lastY, lastX + i, lastY + 5, Black);
		}
		/* Set new paddle */
		if((adc_Xposition + i) < lastX || (adc_Xposition + i) > (lastX + 40))
		{
			LCD_DrawLine(adc_Xposition + i, adc_Yposition, adc_Xposition + i, adc_Yposition + 5, Green);
		}
	}
	
}


void ADC_IRQHandler(void) {
  	
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result */
  if(AD_current != AD_last)
	{
		AD_last = AD_current;
		MovePotentiometer();
		MoveBall();
  }	
	
}
