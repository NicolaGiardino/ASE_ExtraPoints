#include "functs.h"

#define MAX_POT MAX_X - 6
#define MIN_POT 6

extern int start, reset, stop;

int score = 0;
int record = 100;

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
		uint8_t ascii[8];
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

uint32_t ASCIItoUnsig(uint8_t *str, uint32_t size)
{
	size_t i;
	uint32_t num = 0;
	for(i = 0; i < size; i++)
	{
		if(str[i] > 47 && str[i] < 58)
		{
				num += (uint32_t)(str[i] - 48) * upow(10, i);
		}
	}
	
}

void PlayGame()
{
	int c = 0;
	while(1)
	{
		
		
		while(stop)
		{
			LED_Off(3);
		}
		LED_On(3);
		
		if(c == 100)
		{
			c = 0;
			reset = 1;
			start = 0;
			while(start == 0)
			{
				LED_On(4);
				PutChar(MAX_X/2 - 50, MAX_Y / 2, 'Y', White, Black);
				PutChar(MAX_X/2 - 40, MAX_Y / 2, 'o', White, Black);
				PutChar(MAX_X/2 - 30, MAX_Y / 2, 'u', White, Black);
				PutChar(MAX_X/2 - 20, MAX_Y / 2, ' ', White, Black);
				PutChar(MAX_X/2 - 10, MAX_Y / 2, 'L', White, Black);
				PutChar(MAX_X/2, MAX_Y / 2, 'o', White, Black);
				PutChar(MAX_X/2 + 10, MAX_Y / 2, 's', White, Black);
				PutChar(MAX_X/2 + 20, MAX_Y / 2, 'e', White, Black);
			}
		}
		c++;
		LED_Off(4);
		
	}
	
}