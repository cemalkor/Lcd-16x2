#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"
#include <stdio.h>
#include "stdarg.h"
#include "stdbool.h"

typedef struct
{
	GPIO_TypeDef *Rs_Gpiox;
	uint32_t Rs_Pinx;
	GPIO_TypeDef *Rw_Gpiox;
	uint32_t Rw_Pinx;
	GPIO_TypeDef *En_Gpiox;
	uint32_t En_Pinx;
	GPIO_TypeDef *Bl_Gpiox;
	uint32_t Bl_Pinx;

	GPIO_TypeDef *D0_Gpiox;
	uint32_t D0_Pinx;
	GPIO_TypeDef *D1_Gpiox;
	uint32_t D1_Pinx;
	GPIO_TypeDef *D2_Gpiox;
	uint32_t D2_Pinx;
	GPIO_TypeDef *D3_Gpiox;
	uint32_t D3_Pinx;

	GPIO_TypeDef *D4_Gpiox;
	uint32_t D4_Pinx;
	GPIO_TypeDef *D5_Gpiox;
	uint32_t D5_Pinx;
	GPIO_TypeDef *D6_Gpiox;
	uint32_t D6_Pinx;
	GPIO_TypeDef *D7_Gpiox;
	uint32_t D7_Pinx;
} Lcd_Gpio_TypeDefStruct;

typedef struct
{
	Lcd_Gpio_TypeDefStruct Lcd_Gpio;
	TIM_HandleTypeDef tim;
	uint8_t Lock;
    uint8_t _displayfunction;
    uint8_t _displaycontrol;
    uint8_t _displaymode;
    uint8_t _numlines;
    uint8_t _row_offsets[4];
    uint8_t _backlightLevel;
} Lcd_Handle_TypeDefStruct;

// Commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// Flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// Flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// Flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// Flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

void Lcd_Delay(Lcd_Handle_TypeDefStruct *Handle, uint16_t us);
void Lcd_Send_String(Lcd_Handle_TypeDefStruct *Handle, char *str);
void Lcd_Printf(Lcd_Handle_TypeDefStruct *Handle, char *fmt, ...);
void Lcd_Init(Lcd_Handle_TypeDefStruct *Handle, uint8_t fourbitmode);

#ifdef Analog
void Lcd_SetBacklight(uint8_t level, bool userDefined);
void Lcd_ContrastLevel(uint8_t level);
#endif

void Lcd_Clear(Lcd_Handle_TypeDefStruct *Handle);
void Lcd_Home(Lcd_Handle_TypeDefStruct *Handle);

void Lcd_NoDisplay(Lcd_Handle_TypeDefStruct *Handle);
void Lcd_Display(Lcd_Handle_TypeDefStruct *Handle);
void Lcd_NoBlink(Lcd_Handle_TypeDefStruct *Handle);
void Lcd_Blink(Lcd_Handle_TypeDefStruct *Handle);
void Lcd_NoCursor(Lcd_Handle_TypeDefStruct *Handle);
void Lcd_Cursor(Lcd_Handle_TypeDefStruct *Handle);
void Lcd_ScrollDisplayLeft(Lcd_Handle_TypeDefStruct *Handle);
void Lcd_ScrollDisplayRight(Lcd_Handle_TypeDefStruct *Handle);
void Lcd_LeftToRight(Lcd_Handle_TypeDefStruct *Handle);
void Lcd_RightToLeft(Lcd_Handle_TypeDefStruct *Handle);
void Lcd_Autoscroll(Lcd_Handle_TypeDefStruct *Handle);
void Lcd_NoAutoscroll(Lcd_Handle_TypeDefStruct *Handle);

void Lcd_CreateChar(Lcd_Handle_TypeDefStruct *Handle, uint8_t, uint8_t[], bool userAdded);
void Lcd_SetCursor(Lcd_Handle_TypeDefStruct *Handle, uint8_t, uint8_t);
size_t Lcd_Write(Lcd_Handle_TypeDefStruct *Handle, uint8_t);
void Lcd_Command(Lcd_Handle_TypeDefStruct *Handle, uint8_t);

void Lcd_Send(Lcd_Handle_TypeDefStruct *Handle, uint8_t, uint8_t);

#endif /* INC_LCD_H_ */
