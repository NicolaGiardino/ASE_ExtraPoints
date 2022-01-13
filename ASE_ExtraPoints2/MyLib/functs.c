#include "functs.h"
#include "../GLCD/AsciiLib.h"
#include "../timer/timer.h"

/* Defined in the RIT timer library */
extern int start, reset, stop;

/* Defined in the adc library */
extern uint16_t adc_Xposition, adc_Yposition;
extern uint16_t bot_Xposition, bot_Yposition;

/* Old position of the ball */
uint16_t x_old = MAX_X - 6;
uint16_t y_old = MAX_Y / 2;

/* Actual position of the ball */
uint16_t ball_Xpos;
uint16_t ball_Ypos;

int score[2] = {0, 0};

/********************************************************************************
*                                                                               *
* FUNCTION NAME: PutCharReverse 	                                              *
*                                                                               *
* PURPOSE: Print a reverse char on the LCD																			*
* ARGUMENT LIST:                                                                *
*                                                                               *
* Argument  Type         IO     Description                                     *
* --------- --------     --     ---------------------------------               *
* Xpos			uint16_t		 I			Initial X position															*
*	Ypos			uint16_t		 I			Initial Y position															*
*	ASCI			uint8_t			 I			Char to print on the LCD												*
*	charColor	uint16_t		 I			Color to print the number												*
*	bkColor		uint16_t		 I			Color to print the background										*
*																																								*
* RETURN VALUE: void                                                            *
*                                                                               *
********************************************************************************/

void PutCharReverse( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor, uint16_t bkColor )
{
	uint16_t i, j;
    uint8_t buffer[16], tmp_char;
    GetASCIICode(buffer,ASCI);  /* 取字模数据 */
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[15 - i];
        for( j=0; j<8; j++ )
        {
            if( ((tmp_char >> (j)) & 0x01) == 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );  /* 字符颜色 */
            }
            else
            {
                LCD_SetPoint( Xpos + j, Ypos + i, bkColor );  /* 背景颜色 */
            }
        }
    }
}

/********************************************************************************
*                                                                               *
* FUNCTION NAME: GUI_Text_Reverse                                               *
*                                                                               *
* PURPOSE: Print a reverse string on the LCD																		*
* ARGUMENT LIST:                                                                *
*                                                                               *
* Argument  Type         IO     Description                                     *
* --------- --------     --     ---------------------------------               *
* Xpos			uint16_t		 I			Initial X position															*
*	Ypos			uint16_t		 I			Initial Y position															*
*	str				uint8_t*		 I			String to print on the LCD											*
*	Color		  uint16_t		 I			Color to print the number												*
*	bkColor		uint16_t		 I			Color to print the background										*
*																																								*
* RETURN VALUE: void                                                            *
*                                                                               *
********************************************************************************/

void GUI_Text_Reverse(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint16_t Color, uint16_t bkColor)
{
    uint8_t TempChar;
		size_t i = 0;
    do
    {
				i++;
        TempChar = *str++;     
    }
    while ( *str != 0 );
		TempChar = *str--;
		do
		{
				TempChar = *str--;
				PutCharReverse( Xpos, Ypos, TempChar, Color, bkColor );    
        if( Xpos < MAX_X - 8 )
        {
            Xpos += 8;
        } 
        else if ( Ypos < MAX_Y - 16 )
        {
            Xpos = 0;
            Ypos += 16;
        }   
        else
        {
            Xpos = 0;
            Ypos = 0;
        } 
				i--;
		} while( i != 0);
}

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
		sprintf(ascii,"%d",number);
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
* FUNCTION NAME: LCD_PutInt_Reverse                                             *
*                                                                               *
* PURPOSE: Print a reverse number on the LCD																		*
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
void LCD_PutInt_Reverse(uint16_t Xpos, uint16_t Ypos, int number, uint16_t charColor, uint16_t bkColor)
{
		char ascii[8];
		sprintf(ascii,"%d",number);
		if(number < 10)
		{
			PutCharReverse(Xpos, Ypos, ascii[0], charColor, bkColor);
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
void IncrementScore(uint16_t player)
{
	score[player] += 1;
	if(player == USER)
	{
		LCD_PutInt(6, MAX_Y / 2, score[USER], White, Black);
	}
	else if(player == BOT)
	{
		LCD_PutInt_Reverse(MAX_X - 35 - 6, MAX_Y / 2, score[BOT], White, Black);
	}
	if(score[player] == 5)
	{
		GameLost(player);
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
		static uint16_t adc_Xold, bot_Xold;
		static int speed;
		size_t i;
		for(i = 0; i < 5; i++)
		{
			/* Delete previous ball */
			LCD_DrawLine(x_old - i, y_old - 4, x_old - i, y_old, Black);
		}
		for(i = 0; i < 5; i++)
		{
			/* Draw the ball */
			LCD_DrawLine(ball_Xpos - i, ball_Ypos - 4, ball_Xpos - i, ball_Ypos, Green);
		}
		
		/* Calculate next position */
		if(ball_Xpos >= MAX_BALLX || (ball_Xpos - 4) <= MIN_BALLX || 
			((ball_Ypos < (adc_Yposition - 4 && (ball_Ypos - 4) > adc_Yposition)) 
			&& (ball_Xpos == (adc_Xposition + 39) || (ball_Xpos - 4) == adc_Xposition)))
		{
			/* If the ball's in one of the two top edges */
			if(ball_Ypos - 4 <= MIN_BALLY)
			{
				x_new = x_old;
				y_new = y_old;
				disable_timer(0);
				reset_timer(0);
				init_timer(0,1263);
				enable_timer(0);
			}
			/* If the ball is on one of the two potentiometer top angles */
			else if(ball_Xpos == adc_Xposition && (ball_Ypos == (adc_Yposition - 4 || (ball_Ypos - 4) == adc_Yposition)))
			{
				x_new = x_old;
				y_new = y_old;
				
				//IncrementScore(USER);
				disable_timer(0);
				reset_timer(0);
				init_timer(0,1062);
				enable_timer(0);
			}
			/* If the ball's on one of the two lateral walls or on the side of the paddle*/
			else
			{
				x_new = x_old;
				y_new = 2 * ball_Ypos - y_old;
				disable_timer(0);
				reset_timer(0);
				init_timer(0,1263);
				enable_timer(0);
			}
			
			
		}
		/* If the ball is on */
		else if(ball_Ypos - 4 <= MIN_BALLY)
		{
			x_new = 2 * ball_Xpos - x_old;
			y_new = y_old;
			/* Low pitch tone */
			disable_timer(0);
			reset_timer(0);
			init_timer(0,1263);
			enable_timer(0);
		}
		/* if the ball is touching the top of the USER paddle */
		else if((ball_Ypos <= adc_Yposition - 4 && ball_Ypos > adc_Yposition - 9) && (ball_Xpos > adc_Xposition && (ball_Xpos - 4) < (adc_Xposition + 39)))
		{
			/* 
			 * The next position varies on the speed of the paddle, 
			 * calculated using its previous and next position 
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
			disable_timer(0);
			reset_timer(0);
			init_timer(0,1062);
			enable_timer(0);
		}
		/* if the ball is touching the top of the BOT paddle */
		else if((ball_Ypos - 4 <= bot_Yposition && ball_Ypos - 4 > bot_Yposition - 9) && (ball_Xpos > bot_Xposition && (ball_Xpos - 4) < (bot_Xposition + 39)))
		{
			/* 
			 * The next position varies on the speed of the paddle, 
			 * calculated using its previous and next position 
			 */
			speed = (int)(bot_Xposition - bot_Xold);
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
			disable_timer(0);
			reset_timer(0);
			init_timer(0,1062);
			enable_timer(0);
		}
		/* In each other case, when the ball is in the middle of the display */
		else
		{
			x_new = 2 * ball_Xpos - x_old;
			y_new = 2 * ball_Ypos - y_old;
		}
		
		/* Just to be sure that the number is still visible */
		if(ball_Xpos < 27 && (ball_Ypos < MAX_Y / 2 && ball_Ypos > MAX_Y / 2 + 9))
		{
			LCD_PutInt(6, MAX_Y / 2, score[USER], White, Black);
		}
		else if(ball_Xpos > MAX_X - 35 - 6  && (ball_Ypos < MAX_Y / 2 && ball_Ypos > MAX_Y / 2 + 9))
		{
			LCD_PutInt(MAX_X - 35 - 6, MAX_Y / 2, score[BOT], White, Black);
		}
		/* Just to keep the integrity of the game environment */
		if(x_old <= MIN_BALLX)
		{
			for(i = 0; i < 5; i++)
			{
				LCD_DrawLine(i, 0, i, adc_Yposition, Red);
			}
		}
		else if(x_old >= MAX_BALLX)
		{
			for(i = 0; i < 5; i++)
			{
				LCD_DrawLine(MAX_X - 1 - i, 0, MAX_X - 1 - i, adc_Yposition, Red);
			}
		}
		else if(y_old - 4 < MIN_BALLY)
		{
			for(i = 0; i < 5; i++)
			{
				LCD_DrawLine(0, i, MAX_X - 1, i, Red);
			}
		}
		
		/* Update the values */
		x_old = ball_Xpos;
		y_old = ball_Ypos;
		ball_Xpos = x_new;
		ball_Ypos = y_new;
		adc_Xold = adc_Xposition;
		
		/* If the position is lower than the end of the paddle, than it's point to the respective player */
		if(ball_Ypos - 4 > MAX_BALLY)
		{
			IncrementScore(BOT);
			InitBall();
		} 
		else if(ball_Ypos < MAX_BALL_BOT)
		{
			IncrementScore(USER);
			InitBall();
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
void GameLost(uint16_t player)
{
			/* Put reset to 1 and start to zero, 
			 * then print the losing message
			 * finally wait for the start button to be pushed,
			 * resetting the game
			 */
			reset = 1;
			start = 0;
			if(player == USER)
			{
				GUI_Text(MAX_X/2 - 50, MAX_Y / 2 + 50, "You Win", White, Black);
				GUI_Text_Reverse(MAX_X/2 - 50, MAX_Y / 2 - 50, "You Lose", White, Black);
			}
			else
			{
				GUI_Text_Reverse(MAX_X/2 - 50, MAX_Y / 2 - 50, "You Win", White, Black);
				GUI_Text(MAX_X/2 - 50, MAX_Y / 2 + 50, "You Lose", White, Black);
			}
			disable_timer(0);
			reset_timer(0);
			init_timer(0,2048);
			enable_timer(0);
			GUI_Text(MAX_X/2 - 100, MAX_Y / 2 + 15, "Press INT0 to Reset", White, Black);
			NVIC_EnableIRQ(EINT0_IRQn);
}
