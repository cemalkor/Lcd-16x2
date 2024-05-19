#include <Lcd.h>

bool f196 = 0;
bool f197 = 0;
uint8_t cTr[8] = { 0b00000, 0b01110, 0b10001, 0b10000, 0b10000, 0b10001, 0b01110, 0b00100 }; // Büyük küçük Ç
uint8_t GTr[8] = { 0b01110, 0b00000, 0b01110, 0b10001, 0b10000, 0b10111, 0b10001, 0b01111 }; // Büyük Ğ
uint8_t STr[8] = { 0b00000, 0b00000, 0b01110, 0b10000, 0b01110, 0b00001, 0b01110, 0b00100 }; // Büyük küçük Ş
uint8_t gTr[8] = { 0b01110, 0b00000, 0b01111, 0b10001, 0b10001, 0b01111, 0b00001, 0b01110 }; // Küçük ğ
uint8_t ITr[8] = { 0b00100, 0b00000, 0b01110, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110 }; // Büyük İ
uint8_t iTr[8] = { 0b00000, 0b00000, 0b01100, 0b00100, 0b00100, 0b00100, 0b01110, 0b00000 }; // Küçük ı

static void Lcd_Lock(Lcd_Handle_TypeDefStruct *Handle)
{
	while (Handle->Lock)
	{
		HAL_Delay(1);
	}
	Handle->Lock = 1;
}

static void Lcd_UnLock(Lcd_Handle_TypeDefStruct *Handle)
{
	Handle->Lock = 0;
}

static void Lcd_SetRowOffsets(Lcd_Handle_TypeDefStruct *Handle,int row0, int row1, int row2, int row3)
{
	Handle->_row_offsets[0] = row0;
	Handle->_row_offsets[1] = row1;
	Handle->_row_offsets[2] = row2;
	Handle->_row_offsets[3] = row3;
}

static void Lcd_PulseEnable(Lcd_Handle_TypeDefStruct *Handle)
{
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.En_Gpiox, Handle->Lcd_Gpio.En_Pinx, 0);
	Lcd_Delay(Handle, 1);
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.En_Gpiox, Handle->Lcd_Gpio.En_Pinx, 1);
	Lcd_Delay(Handle, 1);    // enable pulse must be >450ns
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.En_Gpiox, Handle->Lcd_Gpio.En_Pinx, 0);
	Lcd_Delay(Handle, 100);   // commands need > 37us to settle
}

static void Lcd_Write4bits(Lcd_Handle_TypeDefStruct *Handle, uint8_t value)
{

	HAL_GPIO_WritePin(Handle->Lcd_Gpio.D7_Gpiox, Handle->Lcd_Gpio.D7_Pinx, ((value >> 3) & 0x01));
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.D6_Gpiox, Handle->Lcd_Gpio.D6_Pinx, ((value >> 2) & 0x01));
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.D5_Gpiox, Handle->Lcd_Gpio.D5_Pinx, ((value >> 1) & 0x01));
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.D4_Gpiox, Handle->Lcd_Gpio.D4_Pinx, ((value >> 0) & 0x01));

	Lcd_PulseEnable(Handle);
}

static void Lcd_Write8bits(Lcd_Handle_TypeDefStruct *Handle, uint8_t value)
{
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.D7_Gpiox, Handle->Lcd_Gpio.D7_Pinx, ((value >> 7) & 0x01));
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.D6_Gpiox, Handle->Lcd_Gpio.D6_Pinx, ((value >> 6) & 0x01));
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.D5_Gpiox, Handle->Lcd_Gpio.D5_Pinx, ((value >> 5) & 0x01));
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.D4_Gpiox, Handle->Lcd_Gpio.D4_Pinx, ((value >> 4) & 0x01));
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.D3_Gpiox, Handle->Lcd_Gpio.D3_Pinx, ((value >> 3) & 0x01));
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.D2_Gpiox, Handle->Lcd_Gpio.D2_Pinx, ((value >> 2) & 0x01));
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.D1_Gpiox, Handle->Lcd_Gpio.D1_Pinx, ((value >> 1) & 0x01));
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.D0_Gpiox, Handle->Lcd_Gpio.D0_Pinx, ((value >> 0) & 0x01));
	Lcd_PulseEnable(Handle);
}

static void Lcd_Begin(Lcd_Handle_TypeDefStruct *Handle, uint8_t cols, uint8_t lines, uint8_t dotsize)
{
	if (lines > 1)
	{
		Handle->_displayfunction |= LCD_2LINE;
	}
	Handle->_numlines = lines;

	Lcd_SetRowOffsets(Handle,0x00, 0x40, 0x00 + cols, 0x40 + cols);

	// For some 1 line displays you can select a 10 pixel high font
	if ((dotsize != LCD_5x8DOTS) && (lines == 1))
	{
		Handle->_displayfunction |= LCD_5x10DOTS;
	}

	HAL_Delay(100);

	HAL_GPIO_WritePin(Handle->Lcd_Gpio.Rs_Gpiox, Handle->Lcd_Gpio.Rs_Pinx, 0);
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.En_Gpiox, Handle->Lcd_Gpio.En_Pinx, 0);
	HAL_GPIO_WritePin(Handle->Lcd_Gpio.Rw_Gpiox, Handle->Lcd_Gpio.Rw_Pinx, 0);

	if (!(Handle->_displayfunction & LCD_8BITMODE))
	{
		// Start in 8bit mode, try to set 4 bit mode
		Lcd_Write4bits(Handle, 0x03);
		HAL_Delay(5); // wait min 4.1ms

		Lcd_Write4bits(Handle, 0x03);
		HAL_Delay(5); // wait min 4.1ms

		Lcd_Write4bits(Handle, 0x03);
		HAL_Delay(2);

		// finally, set to 4-bit interface
		Lcd_Write4bits(Handle, 0x02);
	}
	else
	{
		// Send function set command sequence
		Lcd_Command(Handle, LCD_FUNCTIONSET | Handle->_displayfunction);
		HAL_Delay(5);  // wait more than 4.1ms

		Lcd_Command(Handle, LCD_FUNCTIONSET | Handle->_displayfunction);
		HAL_Delay(2);

		Lcd_Command(Handle, LCD_FUNCTIONSET | Handle->_displayfunction);
	}

	Lcd_Command(Handle, LCD_FUNCTIONSET | Handle->_displayfunction);

	Handle->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	Lcd_Display(Handle);

	Lcd_Clear(Handle);

	// Initialize to default text direction
	Handle->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// Set the entry mode
	Lcd_Command(Handle, LCD_ENTRYMODESET | Handle->_displaymode);
	Lcd_CreateChar(Handle, 0, cTr, 0); // Büyük küçük Ç
	Lcd_CreateChar(Handle, 1, GTr, 0); // Büyük Ğ
	Lcd_CreateChar(Handle, 2, STr, 0); // Büyük küçük Ş
	Lcd_CreateChar(Handle, 3, gTr, 0); // Küçük ğ
	Lcd_CreateChar(Handle, 4, ITr, 0); // Büyük İ
	Lcd_CreateChar(Handle, 5, iTr, 0); // Küçük ı

}

void Lcd_Delay(Lcd_Handle_TypeDefStruct *Handle, uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&Handle->tim, 0);
	while (__HAL_TIM_GET_COUNTER(&Handle->tim) < us);
}

void Lcd_Send_String(Lcd_Handle_TypeDefStruct *Handle, char *str)
{
	while (*str)
		Lcd_Write(Handle, *str++);
}

void Lcd_Printf(Lcd_Handle_TypeDefStruct *Handle, char *fmt, ...)
{
	va_list args;
	char buffer[32];

	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);

	Lcd_Send_String(Handle, buffer);
}

void Lcd_Init(Lcd_Handle_TypeDefStruct *Handle, uint8_t fourbitmode)
{
	if (fourbitmode)
	{
		Handle->_displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
	}
	else
	{
		Handle->_displayfunction = LCD_8BITMODE | LCD_2LINE | LCD_5x8DOTS;
	}

	Lcd_Begin(Handle, 16, 2, LCD_5x8DOTS);
}

#ifdef Analog
void Lcd_SetBacklight(uint8_t level, bool userDefined)
{
	
}

void Lcd_ContrastLevel(uint8_t level)
{
	
}
#endif

void Lcd_Clear(Lcd_Handle_TypeDefStruct *Handle)
{
	Lcd_Command(Handle, LCD_CLEARDISPLAY); // Clear display, set cursor position to zero
	HAL_Delay(2);  // This command takes a long time!
}

void Lcd_Home(Lcd_Handle_TypeDefStruct *Handle)
{
	Lcd_Command(Handle, LCD_RETURNHOME);  // Set cursor position to zero
	HAL_Delay(2);  // This command takes a long time!
}

void Lcd_SetCursor(Lcd_Handle_TypeDefStruct *Handle, uint8_t col, uint8_t row)
{
	const size_t max_lines = sizeof(Handle->_row_offsets) / sizeof(*Handle->_row_offsets);
	if (row >= max_lines)
	{
		row = max_lines - 1;
	}
	if (row >= Handle->_numlines)
	{
		row = Handle->_numlines - 1;
	}

	Lcd_Command(Handle, LCD_SETDDRAMADDR | (col + Handle->_row_offsets[row]));
}

void Lcd_NoDisplay(Lcd_Handle_TypeDefStruct *Handle)
{
#ifdef Analog
	setBacklight(0, 0);
#endif
	Handle->_displaycontrol &= ~LCD_DISPLAYON;
	Lcd_Command(Handle, LCD_DISPLAYCONTROL | Handle->_displaycontrol);
}

void Lcd_Display(Lcd_Handle_TypeDefStruct *Handle)
{
#ifdef Analog
	setBacklight(Handle->_backlightLevel, 1);
#endif
	Handle->_displaycontrol |= LCD_DISPLAYON;
	Lcd_Command(Handle, LCD_DISPLAYCONTROL | Handle->_displaycontrol);
}

void Lcd_NoCursor(Lcd_Handle_TypeDefStruct *Handle)
{
	Handle->_displaycontrol &= ~LCD_CURSORON;
	Lcd_Command(Handle, LCD_DISPLAYCONTROL | Handle->_displaycontrol);
}

void Lcd_Cursor(Lcd_Handle_TypeDefStruct *Handle)
{
	Handle->_displaycontrol |= LCD_CURSORON;
	Lcd_Command(Handle, LCD_DISPLAYCONTROL | Handle->_displaycontrol);
}

void Lcd_NoBlink(Lcd_Handle_TypeDefStruct *Handle)
{
	Handle->_displaycontrol &= ~LCD_BLINKON;
	Lcd_Command(Handle, LCD_DISPLAYCONTROL | Handle->_displaycontrol);
}

void Lcd_Blink(Lcd_Handle_TypeDefStruct *Handle)
{
	Handle->_displaycontrol |= LCD_BLINKON;
	Lcd_Command(Handle, LCD_DISPLAYCONTROL | Handle->_displaycontrol);
}

void Lcd_ScrollDisplayLeft(Lcd_Handle_TypeDefStruct *Handle)
{
	Lcd_Command(Handle, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void Lcd_ScrollDisplayRight(Lcd_Handle_TypeDefStruct *Handle)
{
	Lcd_Command(Handle, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void Lcd_LeftToRight(Lcd_Handle_TypeDefStruct *Handle)
{
	Handle->_displaymode |= LCD_ENTRYLEFT;
	Lcd_Command(Handle, LCD_ENTRYMODESET | Handle->_displaymode);
}

void Lcd_RightToLeft(Lcd_Handle_TypeDefStruct *Handle)
{
	Handle->_displaymode &= ~LCD_ENTRYLEFT;
	Lcd_Command(Handle, LCD_ENTRYMODESET | Handle->_displaymode);
}

void Lcd_Autoscroll(Lcd_Handle_TypeDefStruct *Handle)
{
	Handle->_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	Lcd_Command(Handle, LCD_ENTRYMODESET | Handle->_displaymode);
}

void Lcd_NoAutoscroll(Lcd_Handle_TypeDefStruct *Handle)
{
	Handle->_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	Lcd_Command(Handle, LCD_ENTRYMODESET | Handle->_displaymode);
}

void Lcd_CreateChar(Lcd_Handle_TypeDefStruct *Handle, uint8_t location, uint8_t charmap[], bool userAdded)
{
	if (userAdded && location < 6)
		return;

	location &= 0x7; // We only have 8 locations 0-7
	Lcd_Command(Handle, LCD_SETCGRAMADDR | (location << 3));
	for (uint8_t i = 0; i < 8; i++)
	{
		Lcd_Write(Handle, charmap[i]);
	}

}

void Lcd_Command(Lcd_Handle_TypeDefStruct *Handle, uint8_t value)
{
	Lcd_Send(Handle, value, 0);
}

size_t Lcd_Write(Lcd_Handle_TypeDefStruct *Handle, uint8_t val)
{
	bool sent = 0;

	if (val == 195)
		sent = 0;
	else if (val == 182)
	{
		sent = 1;
		val = 239;
	}
	else if (val == 156)
	{
		sent = 1;
		val = 245;
	}
	else if (val == 188)
	{
		sent = 1;
		val = 245;
	}
	else if (val == 150)
	{
		sent = 1;
		val = 239;
	}
	else if (val == 135)
	{
		sent = 1;
		val = 0;
	} //Ç
	else if (val == 167)
	{
		sent = 1;
		val = 0;
	} //ç
	else if (val == 196)
	{
		sent = 0;
		f196 = 1;
	}
	else if (val == 197)
	{
		sent = 0;
		f197 = 1;
	}
	else if (val == 158)
	{
		sent = 1;
		if (f196 == 1)
		{
			val = 1;
			f196 = 0;
		}
		else if (f197 == 1)
		{
			val = 2;
			f197 = 0;
		}
		else
			sent = 0;
	}
	else if (val == 159)
	{
		sent = 1;
		if (f196 == 1)
		{
			val = 3;
			f196 = 0;
		}
		else if (f197 == 1)
		{
			val = 2;
			f197 = 0;
		}
		else
			sent = 0;
	}
	else if (val == 176)
	{
		if (f196 == 1)
		{
			sent = 1;
			val = 4;
			f196 = 0;
		}
		else
			sent = 0;
	}
	else if (val == 177)
	{
		if (f196 == 1)
		{
			sent = 1;
			val = 5;
			f196 = 0;
		}
		else
			sent = 0;
	}
	else
		sent = 1;
	if (sent == 1)
		Lcd_Send(Handle, val, 1);
	return 1;
}

void Lcd_Send(Lcd_Handle_TypeDefStruct *Handle, uint8_t value, uint8_t mode)
{
	Lcd_Lock(Handle);

	HAL_GPIO_WritePin(Handle->Lcd_Gpio.Rs_Gpiox, Handle->Lcd_Gpio.Rs_Pinx, mode);

	HAL_GPIO_WritePin(Handle->Lcd_Gpio.Rw_Gpiox, Handle->Lcd_Gpio.Rw_Pinx, 0);

	if (Handle->_displayfunction & LCD_8BITMODE)
	{
		Lcd_Write8bits(Handle, value);
	}
	else
	{
		Lcd_Write4bits(Handle, value >> 4);
		Lcd_Write4bits(Handle, value);
	}

	Lcd_UnLock(Handle);
}

