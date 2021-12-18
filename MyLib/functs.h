#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../led/led.h"

uint32_t ASCIItoUnsig(uint8_t *str, uint32_t size);
void LCD_PutInt(uint16_t Xpos, uint16_t Ypos, int number, uint16_t charColor, uint16_t bkColor);
void PlayGame();
void GameLost();
void DrawLateralLines();