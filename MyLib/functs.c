#include "functs.h"

/* Potentiometer edge positions */
#define MAX_POT MAX_X - 6
#define MIN_POT 6

/* Ball edge positions on both axis */
#define MAX_BALLX MAX_X - 6
#define MIN_BALLX 6

#define MIN_BALLY 6
#define MAX_BALLY MAX_Y - 33

/* Defined in the RIT timer library */
extern int start, reset, stop;

/* Defined in the adc library */
extern uint16_t adc_Xposition, adc_Yposition;

/* Old position of the ball */
uint16_t x_old = MAX_X - 6;
uint16_t y_old = MAX_Y / 2;

/* Actual position of the ball */
uint16_t ball_Xpos;
uint16_t ball_Ypos;

int score = 0;
int record = 100;


/********************************************************************************
*                                                                               *
* FUNCTION NAME: DrawLateralLines                                               *
*                                                                               *
* PURPOSE: Draw the red lateral lines																						*
* ARGUMENT LIST:                                                                *
*                                                                               *
* Argument  Type         IO     Description                                     *
* --------- --------     --     ---------------------------------               *
*                                                                               *
* RETURN VALUE: void                                                            *
*                                                                               *
********************************************************************************/
void DrawLateralLines()
{
	size_t i;
	
	for(i = 0; i < 5; i++)
	{
		LCD_DrawLine(i, 0, i, MAX_Y - 1, Red);
		LCD_DrawLine(0, i, MAX_X - 1, i, Red);
		LCD_DrawLine(MAX_X - 1 - i, 0, MAX_X - 1 - i, MAX_Y - 1, Red);
	}
}

/********************************************************************************
*                                                                               *
* FUNCTION NAME: LCD_PutInt				                                              *
*                                                                               *
* PURPOSE: Print a number on the LCD																						*
* ARGUMENT LIST:                                                                *
*                                                                               *
* Argument  Type         IO     Description                                     *
* --------- --------     --     ---------------------------------               *
* Xpos			uint16_t		 I			Initial X position															*
*	Ypos			uint16_t		 I			Initial Y position															*
*	number		int					 I			Number to print on the LCD											*
*	charColor	uint16_t		 I			Color to print the number												*
*	bkColor		uint16_t		 I			Color to print the background										*
*																																								*
* RETURN VALUE: void                                                            *
*                                                                               *
********************************************************************************/
void LCD_PutInt(uint16_t Xpos, uint16_t Ypos, int number, uint16_t charColor, uint16_t bkColor)
{
		char ascii[8];
		sprintf(ascii,"%d",score);
		if(number < 10)
		{
			PutChar(Xpos, Ypos, ascii[0], charColor, bkColor);
		}
		else if(number < 100)
		{
			PutChar(Xpos, Ypos, ascii[0], charColor, bkColor);
			PutChar(Xpos + 9, Ypos, ascii[1], charColor, bkColor);
		}
		else if(number < 1000)
		{
			PutChar(Xpos, Ypos, ascii[0], charColor, bkColor);
			PutChar(Xpos + 9, Ypos, ascii[1], charColor, bkColor);
			PutChar(Xpos + 18, Ypos, ascii[2], charColor, bkColor);
		}
		else if(number < 10000)
		{
			PutChar(Xpos, Ypos, ascii[0], charColor, bkColor);
			PutChar(Xpos + 9, Ypos, ascii[1], charColor, bkColor);
			PutChar(Xpos + 18, Ypos, ascii[2], charColor, bkColor);
			PutChar(Xpos + 27, Ypos, ascii[3], charColor, bkColor);
		}
}

/********************************************************************************
*                                                                               *
* FUNCTION NAME: IncrementScore		                                              *
*                                                                               *
* PURPOSE: Increment the score of the game																			*
* ARGUMENT LIST:                                                                *
*                                                                               *
* Argument  Type         IO     Description                                     *
* --------- --------     --     ---------------------------------               *
*                                                                               *
* RETURN VALUE: void                                                            *
*                                                                               *
********************************************************************************/
void IncrementScore()
{
	if(score >= 100)
	{
		score += 10;
		record = score;
	}
	else
	{
		score += 5;
	}
}

/********************************************************************************
*                                                                               *
* FUNCTION NAME: InitBall					                                              *
*                                                                               *
* PURPOSE: Initialize the ball's position																				*
* ARGUMENT LIST:                                                                *
*                                                                               *
* Argument  Type         IO     Description                                     *
* --------- --------     --     ---------------------------------               *
*                                                                               *
* RETURN VALUE: void                                                            *
*                                                                               *
********************************************************************************/
void InitBall()
{
		size_t i;
	
		ball_Xpos = MAX_X - 6;
		ball_Ypos = MAX_Y / 2;
	
		for(i = 0; i < 5; i++)
		{
			LCD_DrawLine(x_old - 4, y_old - i, x_old, y_old - i, Black);
			LCD_DrawLine(ball_Xpos - 4, ball_Ypos - i, ball_Xpos, ball_Ypos - i,Green);
		}
	
		x_old = MAX_X - 6;
		y_old = MAX_Y / 2;
		ball_Xpos -=1;
		ball_Ypos +=1;
}

/********************************************************************************
*                                                                               *
* FUNCTION NAME: MoveBall					                                              *
*                                                                               *
* PURPOSE: Function to calculate the next ball's position based on							*
*						where the current position is																				*
* ARGUMENT LIST:                                                                *
*                                                                               *
* Argument  Type         IO     Description                                     *
* --------- --------     --     ---------------------------------               *
*                                                                               *
* RETURN VALUE: void                                                            *
*                                                                               *
********************************************************************************/
void MoveBall()
{
		
	  static uint16_t x_new, y_new;
		static uint16_t adc_Xold;
		static int speed;
		size_t i;
		/* Delete previous ball */
		for(i = 0; i < 5; i++)
		{
			LCD_DrawLine(x_old - 4, y_old - i, x_old, y_old - i, Black);
		}
		/* Draw the ball */
		for(i = 0; i < 5; i++)
		{
			LCD_DrawLine(ball_Xpos - 4, ball_Ypos - i, ball_Xpos, ball_Ypos - i,Green);
		}
		
		/* Calculate next position */
		if(ball_Xpos >= MAX_BALLX || (ball_Xpos - 4) <= MIN_BALLX || 
			((ball_Ypos < (adc_Yposition - 4 && (ball_Ypos - 4) > adc_Yposition)) 
			&& (ball_Xpos == (adc_Xposition + 40) || (ball_Xpos - 4) == adc_Xposition)))
		{
			/* If the ball's in one of the two top edges */
			if(ball_Ypos <= MIN_BALLY)
			{
				x_new = x_old;
				y_new = y_old;
				LPC_DAC->DACR = 400;
			}
			/* If the ball is on one of the two potentiometer top angles */
			else if(ball_Xpos == adc_Xposition && (ball_Ypos == (adc_Yposition - 4 || (ball_Ypos - 4) == adc_Yposition)))
			{
				x_new = x_old;
				y_new = y_old;
				IncrementScore();
				LPC_DAC->DACR = 700<<6;
			}
			/* If the ball's on one of the two lateral walls or on the side of the paddle*/
			else
			{
				x_new = x_old;
				y_new = 2 * ball_Ypos - y_old;
				LPC_DAC->DACR = 400<<6;
			}
		}
		else if(ball_Ypos <= MIN_BALLY)
		{
			x_new = 2 * ball_Xpos - x_old;
			y_new = y_old;
			/* Low pitch tone */
			LPC_DAC->DACR = 400<<6;
		}
		/* if the ball is touching the top of the paddle */
		else if((ball_Ypos == adc_Yposition - 4) && (ball_Xpos > adc_Xposition && (ball_Xpos - 4) < (adc_Xposition + 40)))
		{
			/* The next position varies on the speed of the paddle, 
			 * calculated using its previous and next position 
			 * divided by a factor of 100
			 */
			speed = (int)(adc_Xposition - adc_Xold);
			x_new = (2 * ball_Xpos - x_old);
			y_new = y_old;
			
			if((int)(ball_Xpos - x_old) < 0)
			{
				y_new = (y_new - 1 == y_old) ? y_new : y_new - 1;
			}
			else if((int)(ball_Xpos - x_old) > 0)
			{
				if(speed > 0)
				{
					x_new = (x_new + 1 == x_old) ? x_new : x_new + 1;
				}
				else
				{
					x_new = (x_new - 1 == x_old) ? x_new : x_new - 1;
				}
			}
			
			IncrementScore();
			LPC_DAC->DACR = 700<<6;
		}
		/* In each other case, when the ball is in the middle of the display */
		else
		{
			x_new = 2 * ball_Xpos - x_old;
			y_new = 2 * ball_Ypos - y_old;
			LPC_DAC->DACR = 0;
		}
		
		/* Just to be sure that the number is still visible */
		if(ball_Xpos < 27 && (ball_Ypos < MAX_Y / 2 && ball_Xpos > MAX_Y / 2 + 9))
		{
			LCD_PutInt(6, MAX_Y / 2, score, White, Black);
		}
		/* Just to keep the integrity of the game environment */
		if(x_old <= MIN_BALLX || x_old >= MAX_BALLX || y_old - 4 < MIN_BALLY)
		{
			DrawLateralLines();
		}
		
		/* Update the values */
		x_old = ball_Xpos;
		y_old = ball_Ypos;
		ball_Xpos = x_new;
		ball_Ypos = y_new;
		adc_Xold = adc_Xposition;
		
		/* If the position is lower than the end of the paddle, than it's game over */
		if(ball_Ypos - 4 > MAX_BALLY)
		{
			GameLost();
		}
}

/********************************************************************************
*                                                                               *
* FUNCTION NAME: GameLost					                                              *
*                                                                               *
* PURPOSE: Function to be called when the game is lost,													*
*						that's when the ball goes below the paddle													*
* ARGUMENT LIST:                                                                *
*                                                                               *
* Argument  Type         IO     Description                                     *
* --------- --------     --     ---------------------------------               *
*                                                                               *
* RETURN VALUE: void                                                            *
*                                                                               *
********************************************************************************/
void GameLost()
{
			/* Put reset to 1 and start to zero, 
			 * then print the losing message
			 * finally wait for the start button to be pushed,
			 * resetting the game
			 */
			reset = 1;
			start = 0;
			PutChar(MAX_X/2 - 50, MAX_Y / 2, 'Y', White, Black);
			PutChar(MAX_X/2 - 40, MAX_Y / 2, 'o', White, Black);
			PutChar(MAX_X/2 - 30, MAX_Y / 2, 'u', White, Black);
			PutChar(MAX_X/2 - 20, MAX_Y / 2, ' ', White, Black);
			PutChar(MAX_X/2 - 10, MAX_Y / 2, 'L', White, Black);
			PutChar(MAX_X/2, MAX_Y / 2, 'o', White, Black);
			PutChar(MAX_X/2 + 10, MAX_Y / 2, 's', White, Black);
			PutChar(MAX_X/2 + 20, MAX_Y / 2, 'e', White, Black);
			while(start == 0)
			{}
			LCD_Clear(Black);
			score = 0;
			DrawLateralLines();
			LCD_PutInt(6, MAX_Y / 2, score, White, Black);
			InitBall();
}

/********************************************************************************
*                                                                               *
* FUNCTION NAME: PlayGame					                                              *
*                                                                               *
* PURPOSE: Main function for the game																						*
* ARGUMENT LIST:                                                                *
*                                                                               *
* Argument  Type         IO     Description                                     *
* --------- --------     --     ---------------------------------               *
*                                                                               *
* RETURN VALUE: void                                                            *
*                                                                               *
********************************************************************************/
void PlayGame()
{
	while(1)
	{
		
		/* If the game is stopped, it will be put in an infinite loop until started */
		while(stop)
		{}
		
		ADC_start_conversion();
		MoveBall();
		
		
	}
	
}
