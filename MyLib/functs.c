#include "functs.h"

#define MAX_POT MAX_X - 6
#define MIN_POT 6

#define MAX_BALLX MAX_X - 6
#define MIN_BALLX 6

#define MIN_BALLY 6
#define MAX_BALLY MAX_Y - 33

extern int start, reset, stop;
extern uint16_t adc_Xposition, adc_Yposition;

uint16_t x_old = MAX_X - 6;
uint16_t y_old = MAX_Y / 2;

int score = 0;
int record = 100;
uint16_t ball_Xpos;
uint16_t ball_Ypos;

uint16_t SinTable[45] =                                       /*                      */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

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

void LCD_PutInt(uint16_t Xpos, uint16_t Ypos, int number, uint16_t charColor, uint16_t bkColor)
{
		char ascii[8];
		sprintf(ascii,"%d",score);
		if(number < 10)
		{
			PutChar(Xpos, Ypos, ascii[0], charColor, bkColor);
		}
		else if(number >= 10 && number < 100)
		{
			PutChar(Xpos, Ypos, ascii[0], charColor, bkColor);
			PutChar(Xpos + 9, Ypos, ascii[1], charColor, bkColor);
		}
		else if(number >= 100 && number < 1000)
		{
			PutChar(Xpos, Ypos, ascii[0], charColor, bkColor);
			PutChar(Xpos + 9, Ypos, ascii[1], charColor, bkColor);
			PutChar(Xpos + 18, Ypos, ascii[2], charColor, bkColor);
		}
		else if(number >= 1000 && number < 10000)
		{
			PutChar(Xpos, Ypos, ascii[0], charColor, bkColor);
			PutChar(Xpos + 9, Ypos, ascii[1], charColor, bkColor);
			PutChar(Xpos + 18, Ypos, ascii[2], charColor, bkColor);
			PutChar(Xpos + 27, Ypos, ascii[3], charColor, bkColor);
		}
}

uint32_t upow(uint32_t base, uint32_t exp)
{
    int result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

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

void MoveBall()
{
		
	  static uint16_t x_new, y_new;
		static uint16_t factor = 100;
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
		if(ball_Xpos == MAX_BALLX || (ball_Xpos - 4) == MIN_BALLX || 
			((ball_Ypos < (adc_Yposition - 4 && (ball_Ypos - 4) > adc_Yposition)) 
			&& (ball_Xpos == (adc_Xposition + 40) || (ball_Xpos - 4) == adc_Xposition))) /* Case 0 && 1 && 2 && 6(IF3 IF4)*/
		{
			if(ball_Ypos == MIN_BALLY)
			{
				x_new = x_old;
				y_new = y_old;
				LPC_DAC->DACR = 400;
			}
			else if(ball_Xpos == adc_Xposition && (ball_Ypos == (adc_Yposition - 4 || (ball_Ypos - 4) == adc_Yposition))) 
				/* Case angle and IF3 - 4 cases */
			{
				x_new = x_old;
				y_new = y_old;
				IncrementScore();
				LPC_DAC->DACR = 700<<6;
			}
			else
			{
				x_new = x_old;
				y_new = 2 * ball_Ypos - y_old;
				LPC_DAC->DACR = 400<<6;
			}
			
		}
		else if(ball_Ypos == MIN_BALLY) /* Case 3 */
		{
			x_new = 2 * ball_Xpos - x_old;
			y_new = y_old;
			/* Low pitch tone */
			LPC_DAC->DACR = 400<<6;
		}
		else if((ball_Ypos == adc_Yposition - 4) && (ball_Xpos > adc_Xposition && (ball_Xpos - 4) < (adc_Xposition + 40))) /* Case 4 IF1 && IF2*/ 
		{
			/* Speed to be implemented */
			speed = (int)((x_new - x_old) / 2);
			if(speed < 0)
			{
				speed = -1/speed;
			}
			x_new = (2 * ball_Xpos - x_old) * (uint16_t)speed / factor;
			y_new = y_old * (uint16_t)speed / factor;
			IncrementScore();
			LPC_DAC->DACR = 700<<6;
		}
		else /* Case 0 */
		{
			x_new = 2 * ball_Xpos - x_old;
			y_new = 2 * ball_Ypos - y_old;
			LPC_DAC->DACR = 0;
		}
		x_old = ball_Xpos;
		y_old = ball_Ypos;
		ball_Xpos = x_new;
		ball_Ypos = y_new;
		if(ball_Ypos > MAX_BALLY)	/* Case 5 */
		{
			/* Here it fails */
			GameLost();
		}
}

void GameLost()
{
			reset = 1;
			start = 0;
			score = 0;
			LCD_PutInt(6, MAX_Y / 2, score, White, Black);
			PutChar(15, MAX_Y / 2, ' ', Black, Black);
			PutChar(24, MAX_Y / 2, ' ', Black, Black);
			LED_On(4);
			InitBall();
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
			PutChar(MAX_X/2 - 50, MAX_Y / 2, 'Y', Black, Black);
			PutChar(MAX_X/2 - 40, MAX_Y / 2, 'o', Black, Black);
			PutChar(MAX_X/2 - 30, MAX_Y / 2, 'u', Black, Black);
			PutChar(MAX_X/2 - 20, MAX_Y / 2, ' ', Black, Black);
			PutChar(MAX_X/2 - 10, MAX_Y / 2, 'L', Black, Black);
			PutChar(MAX_X/2, MAX_Y / 2, 'o', Black, Black);
			PutChar(MAX_X/2 + 10, MAX_Y / 2, 's', Black, Black);
			PutChar(MAX_X/2 + 20, MAX_Y / 2, 'e', Black, Black);
}

void PlayGame()
{
	while(1)
	{
		
		
		while(stop)
		{}
		
		ADC_start_conversion();
		MoveBall();
		
		LCD_PutInt(6, MAX_Y / 2, score, White, Black);
		
	}
	
}
