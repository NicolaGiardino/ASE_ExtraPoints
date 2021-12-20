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

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

#define MAX_PADDLE 0xD60 /* 641 decimal */
#define MIN_PADDLE 0x281 /* 3424 decimal */

unsigned short AD_current;   
unsigned short AD_last = 0xFF;     /* Last converted value               */

uint16_t adc_Xposition = MAX_X / 2 - 20;
uint16_t adc_Yposition = MAX_Y - 33;

void MovePotentiometer()
{
	size_t i;
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
	for(i = 0; i < 5; i++)
	{
		LCD_DrawLine(adc_Xposition, adc_Yposition - i, adc_Xposition + 40, adc_Yposition - i, Green);
		LCD_DrawLine(6, adc_Yposition - i, adc_Xposition - 1, adc_Yposition - i, Black); /* Clear last paddle */
		LCD_DrawLine(adc_Xposition + 40, adc_Yposition - i, MAX_X - 6, adc_Yposition - i, Black); 
	}
}

void ADC_IRQHandler(void) {
  	
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */
  if(AD_current != AD_last){
		//LED_Off(AD_last*7/0xFFF);	  // ad_last : AD_max = x : 7 		LED_Off((AD_last*7/0xFFF));	
		//LED_On(AD_current*7/0xFFF);	// ad_current : AD_max = x : 7 		LED_On((AD_current*7/0xFFF));
		AD_last = AD_current;
		MovePotentiometer();
  }	
	
}
