/*
 * test_line.c
 *
 *  Created on: 9 mars 2015
 *      Author: colin
 */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

#define JOYSTICK_LEFT       GPIOC, GPIO_Pin_8
#define JOYSTICK_UP         GPIOC, GPIO_Pin_9
#define JOYSTICK_DOWN       GPIOC, GPIO_Pin_10
#define JOYSTICK_RIGHT      GPIOC, GPIO_Pin_11
#define JOYSTICK_PUSH       GPIOC, GPIO_Pin_12
#define RETURN_BUTTON       GPIOC, GPIO_Pin_13

void testLine ()
{
	char x=64,y=32;
	while (1)
	{
		if (GPIO_ReadInputDataBit(JOYSTICK_LEFT) == Bit_RESET)
		{
			// Wait until button is released
			hal_ui_anti_rebonds (JOYSTICK_LEFT);
			x--;
			ssd1306ClearScreen();
			ssd1306DrawLine(64,32,x,y);
			ssd1306DrawPixel(x,y);
			ssd1306DrawPixel(64,32);
			ssd1306Refresh();
		}

		// Joystick down
		if (GPIO_ReadInputDataBit(JOYSTICK_DOWN) == Bit_RESET)
		{
			hal_ui_anti_rebonds (JOYSTICK_DOWN);
			y++;
			ssd1306ClearScreen();
			ssd1306DrawLine(64,32,x,y);
			ssd1306DrawPixel(x,y);
			ssd1306DrawPixel(64,32);
			ssd1306Refresh();
		}
		// Joystick up
		if (GPIO_ReadInputDataBit(JOYSTICK_UP) == Bit_RESET)
		{
			hal_ui_anti_rebonds (JOYSTICK_UP);
			y--;
			ssd1306ClearScreen();
			ssd1306DrawLine(64,32,x,y);
			ssd1306DrawPixel(x,y);
			ssd1306DrawPixel(64,32);
			ssd1306Refresh();
		}

		// Validate button
		if(GPIO_ReadInputDataBit(JOYSTICK_RIGHT) == Bit_RESET)
		{
			hal_ui_anti_rebonds (JOYSTICK_RIGHT);
			x++;
			ssd1306ClearScreen();
			ssd1306DrawLine(64,32,x,y);
			ssd1306DrawPixel(x,y);
			ssd1306DrawPixel(64,32);
			ssd1306Refresh();
		}
	}
}
void testLine2 ()
{
	char x=64,y=32;
	while (1)
	{
		if (GPIO_ReadInputDataBit(JOYSTICK_LEFT) == Bit_RESET)
		{
			// Wait until button is released
			hal_ui_anti_rebonds (JOYSTICK_LEFT);
			y--;
			x=(128-y)/-10;
			ssd1306ClearScreen();
			ssd1306DrawLine(64,32,x,y);
			ssd1306DrawPixel(x,y);
			ssd1306DrawPixel(64,32);
			ssd1306Refresh();
		}

		// Validate button
		if(GPIO_ReadInputDataBit(JOYSTICK_RIGHT) == Bit_RESET)
		{
			hal_ui_anti_rebonds (JOYSTICK_RIGHT);
			y++;
			x=(128-y)/-10;
			ssd1306ClearScreen();
			ssd1306DrawLine(64,32,x,y);
			ssd1306Refresh();
		}
	}
}
