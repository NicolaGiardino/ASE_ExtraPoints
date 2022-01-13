#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../led/led.h"
#include "../adc/adc.h"

/* Player ID */
#define USER	0
#define BOT		1

/* Potentiometer edge positions */
#define MIN_POT 6
#define MAX_POT MAX_X - 6

/* Ball edge positions on both axis */
#define MIN_BALLX 6
#define MAX_BALLX MAX_X - 6

#define MIN_BALLY 6
#define MAX_BALLY MAX_Y - 33

#define MAX_BALL_BOT 31

uint32_t ASCIItoUnsig(uint8_t *str, uint32_t size);
void InitBall(void);
void MoveBall(void);
void LCD_PutInt(uint16_t Xpos, uint16_t Ypos, int number, uint16_t charColor, uint16_t bkColor);
void PlayGame(void);
void GameLost(uint16_t player);
void DrawLateralLines(void);
