/**************************************************************************/
/*!
    @file     ssd1306.h
    @author   K. Townsend (microBuilder.eu)
    @date     22 March 2010
    @version  0.10

    Driver for 128x64 OLED display based on the SSD1306 controller.

    This driver is based on the SSD1306 Library from Limor Fried
    (Adafruit Industries) at: https://github.com/adafruit/SSD1306  
 */
/**************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stdarg.h"

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"
#include "hal/hal_os.h"
#include "oled/ssd1306_peripherals.h"
#include "oled/ssd1306.h"
#include "oled/smallfonts.h"

#define DELAY(ms)     	hal_os_sleep(ms);

unsigned char buffer[SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8]= {
/*0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x80, 0x80, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0xFF,
0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
0x80, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8C, 0x8E, 0x84, 0x00, 0x00, 0x80, 0xF8,
0xF8, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80,
0x00, 0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xC7, 0x01, 0x01,
0x01, 0x01, 0x83, 0xFF, 0xFF, 0x00, 0x00, 0x7C, 0xFE, 0xC7, 0x01, 0x01, 0x01, 0x01, 0x83, 0xFF,
0xFF, 0xFF, 0x00, 0x38, 0xFE, 0xC7, 0x83, 0x01, 0x01, 0x01, 0x83, 0xC7, 0xFF, 0xFF, 0x00, 0x00,
0x01, 0xFF, 0xFF, 0x01, 0x01, 0x00, 0xFF, 0xFF, 0x07, 0x01, 0x01, 0x01, 0x00, 0x00, 0x7F, 0xFF,
0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0xFF,
0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x8F,
0x8F, 0x9F, 0xBF, 0xFF, 0xFF, 0xC3, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC,
0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x01, 0x03, 0x03, 0x03,
0x03, 0x03, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01,
0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00,
0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x03,
0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#if (SSD1306_LCDHEIGHT == 64)
0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x1F, 0x0F,
0x87, 0xC7, 0xF7, 0xFF, 0xFF, 0x1F, 0x1F, 0x3D, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0x7C, 0x7D, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x00, 0x30, 0x30, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xC0, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x1F,
0x0F, 0x07, 0x1F, 0x7F, 0xFF, 0xFF, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xE0,
0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00,
0x00, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0xF0, 0xF8, 0x1C, 0x0E,
0x06, 0x06, 0x06, 0x0C, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0xFC,
0xFE, 0xFC, 0x00, 0x18, 0x3C, 0x7E, 0x66, 0xE6, 0xCE, 0x84, 0x00, 0x00, 0x06, 0xFF, 0xFF, 0x06,
0x06, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x06, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0xC0, 0xF8,
0xFC, 0x4E, 0x46, 0x46, 0x46, 0x4E, 0x7C, 0x78, 0x40, 0x18, 0x3C, 0x76, 0xE6, 0xCE, 0xCC, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F, 0x03,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00,
0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x03, 0x07, 0x0E, 0x0C,
0x18, 0x18, 0x0C, 0x06, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x01, 0x0F, 0x0E, 0x0C, 0x18, 0x0C, 0x0F,
0x07, 0x01, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00,
0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x07,
0x07, 0x0C, 0x0C, 0x18, 0x1C, 0x0C, 0x06, 0x06, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif*/
};

/**************************************************************************/
/*  Private Function Prototypes                                           */
/**************************************************************************/
void   ssd1306SendByte(unsigned char byte);

/**************************************************************************/
/* Macros                                                                 */
/**************************************************************************/
#define swap(a, b){ \
		unsigned char c; \
		c = a; \
		a = b; \
		b = c; \
}

#ifdef HARDWARE_SPI_MACRO
#define CMD(c)  GPIO_ResetBits(CS_GPIO_PORT, CS_PIN); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				GPIO_ResetBits(DC_GPIO_PORT, DC_PIN); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				SPI_I2S_SendData(SPIx, c); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				GPIO_SetBits(CS_GPIO_PORT, CS_PIN); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP");

#define DATA(c) GPIO_ResetBits(CS_GPIO_PORT, CS_PIN); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				GPIO_SetBits(DC_GPIO_PORT, DC_PIN); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				SPI_I2S_SendData(SPIx, c); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				GPIO_ResetBits(DC_GPIO_PORT, DC_PIN); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				GPIO_SetBits(CS_GPIO_PORT, CS_PIN); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP"); \
				asm ("NOP"); asm ("NOP"); asm ("NOP");

#endif

#ifndef HARDWARE_SPI_MACRO
// Note:  These macros could be optimised significantly by setting the
//        GPIO register directly, though the tradeoff is that changing the
//        pin locations is a bit more complicated for absolute beginners
//        since they will need to understand how the bits are set in the
//        appropriate registers, etc.

#define CMD(c)  GPIO_SetBits(CS_GPIO_PORT, CS_PIN); \
				GPIO_ResetBits(CS_GPIO_PORT, CS_PIN); \
				GPIO_ResetBits(DC_GPIO_PORT, DC_PIN); \
				ssd1306SendByte( c );

#define DATA(c) GPIO_SetBits(DC_GPIO_PORT, DC_PIN); \
				ssd1306SendByte( c );
#endif

/**************************************************************************/
/* Private Methods                                                        */
/**************************************************************************/
#ifndef HARDWARE_SPI_MACRO
//Simulates an SPI write using GPIO
//Send CMD
void ssd1306SendByte(unsigned char byte)
{
	signed char i;

	// Make sure clock pin starts high
	GPIO_WriteBit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN, 1);

	// Write from MSB to LSB
	for (i=7; i>=0; i--)
	{
		// Set clock pin low
		GPIO_WriteBit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN, 0);
		// Set data pin high or low depending on the value of the current bit
		GPIO_WriteBit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN, byte & (1 << i) ? 1 : 0);
		// Set clock pin high
		GPIO_WriteBit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN, 1);
	}
}
#endif
/**************************************************************************/
/*!
    @brief  Draws a single graphic character using the supplied font
 */
/**************************************************************************/
static void ssd1306DrawChar(unsigned int x, unsigned int y, unsigned char c, const FONT_DEF *font)
{
	unsigned char col, column[font->u8Width];

	// Check if the requested character is available
	if ((c >= font->u8FirstChar) && (c <= font->u8LastChar))
	{
		// Retrieve appropriate columns from font data
		for (col = 0; col < font->u8Width; col++)
		{
			column[col] = font->au8FontTable[((c - 32) * font->u8Width) + col];    // Get first column of appropriate character
		}
	}
	else
	{
		// Requested character is not available in this font ... send a space instead
		for (col = 0; col < font->u8Width; col++)
		{
			column[col] = 0xFF;    // Send solid space
		}
	}

	// Render each column
	unsigned int xoffset, yoffset;
	for (xoffset = 0; xoffset < font->u8Width; xoffset++)
	{
		for (yoffset = 0; yoffset < (font->u8Height + 1); yoffset++)
		{
			unsigned char bit = 0x00;
			bit = (column[xoffset] << (8 - (yoffset + 1)));     // Shift current row bit left
			bit = (bit >> 7);                                   // Shift current row but right (results in 0x01 for black, and 0x00 for white)
			if (bit)
			{
				ssd1306DrawPixel(x + xoffset, y + yoffset);
			}
		}
	}
}

/**************************************************************************/
/* Public Methods                                                         */
/**************************************************************************/

/**************************************************************************/
/*! 
    @brief Initialises the SSD1306 LCD display
 */
/**************************************************************************/
void ssd1306Init(unsigned char vccstate)
{
	// Initialize peripherals
	ssd1306_GPIO_Config();
	ssd1306_SPI_Config();

	// Reset the LCD
	GPIO_WriteBit(RST_GPIO_PORT, RST_PIN, 1);
	HAL_Delay(10);
	GPIO_WriteBit(RST_GPIO_PORT, RST_PIN, 0);
	HAL_Delay(10);
	GPIO_WriteBit(RST_GPIO_PORT, RST_PIN, 1);
	HAL_Delay(20);

	// Initialisation sequence
	CMD(SSD1306_DISPLAYOFF);                    // 0xAE
	CMD(SSD1306_SETLOWCOLUMN | 0x0);            // low col = 0
	CMD(SSD1306_SETHIGHCOLUMN | 0x0);           // hi col = 0
	CMD(SSD1306_SETSTARTLINE | 0x0);            // line #0
	CMD(SSD1306_SETCONTRAST);                   // 0x81
	CMD(130);									// contrast (0-255)
	if (vccstate == SSD1306_EXTERNALVCC)
	{
		CMD(0x9F);
	}
	else
	{
		CMD(0xCF);
	}
	CMD(0xa1);                                  // setment remap 95 to 0 (?)
	CMD(SSD1306_NORMALDISPLAY);                 // 0xA6
	CMD(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
	CMD(SSD1306_SETMULTIPLEX);                  // 0xA8
	CMD(0x3F);                                  // 0x3F 1/64 duty
	CMD(SSD1306_SETDISPLAYOFFSET);              // 0xD3
	CMD(0x0);                                   // no offset
	CMD(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
	CMD(0x80);                                  // the suggested ratio 0x80
	CMD(SSD1306_SETPRECHARGE);                  // 0xd9
	if (vccstate == SSD1306_EXTERNALVCC)
	{
		CMD(0x22);
	}
	else
	{
		CMD(0xF1);
	}
	CMD(SSD1306_SETCOMPINS);                    // 0xDA
	CMD(0x12);                                  // disable COM left/right remap
	CMD(SSD1306_SETVCOMDETECT);                 // 0xDB
	CMD(0x40);                                  // 0x20 is default?
	CMD(SSD1306_MEMORYMODE);                    // 0x20
	CMD(0x00);                                  // 0x0 act like ks0108
	CMD(SSD1306_SEGREMAP | 0x1);
	CMD(SSD1306_COMSCANDEC);
	CMD(SSD1306_CHARGEPUMP);                    //0x8D
	if (vccstate == SSD1306_EXTERNALVCC)
	{
		CMD(0x10);
	}
	else
	{
		CMD(0x14);
	}

	// Enabled the OLED panel
	CMD(SSD1306_DISPLAYON);
}

/**************************************************************************/
/*! 
    @brief Draws a single pixel in image buffer

    @param[in]  x
                The x position (0..127)
    @param[in]  y
                The y position (0..63)
 */
/**************************************************************************/
void ssd1306DrawPixel(unsigned char x, unsigned char y)
{
	if ((x >= SSD1306_LCDWIDTH) || (y >= SSD1306_LCDHEIGHT))
		return;

	buffer[x+ (y/8)*SSD1306_LCDWIDTH] |= (1 << y%8);
}

/**************************************************************************/
/*!
   @brief Clears a single pixel in image buffer

   @param[in]  x
               The x position (0..127)
   @param[in]  y
               The y position (0..63)
 */
/**************************************************************************/
void ssd1306ClearPixel(unsigned char x, unsigned char y)
{
	if ((x >= SSD1306_LCDWIDTH) || (y >= SSD1306_LCDHEIGHT))
		return;

	buffer[x+ (y/8)*SSD1306_LCDWIDTH] &= ~(1 << y%8);
}

/**************************************************************************/
/*!
   @brief Inverts a single pixel in image buffer

   @param[in]  x
               The x position (0..127)
   @param[in]  y
               The y position (0..63)
 */
/**************************************************************************/
void ssd1306InvertPixel(unsigned char x, unsigned char y)
{
    if ((x >= SSD1306_LCDWIDTH) || (y >= SSD1306_LCDHEIGHT))
    {
        return;
    }
    buffer[x + (y / 8) * SSD1306_LCDWIDTH] ^= (1 << (y % 8));
}

/**************************************************************************/
/*! 
    @brief Gets the value (1 or 0) of the specified pixel from the buffer

    @param[in]  x
                The x position (0..127)
    @param[in]  y
                The y position (0..63)

    @return     1 if the pixel is enabled, 0 if disabled
 */
/**************************************************************************/
unsigned char ssd1306GetPixel(unsigned char x, unsigned char y)
{
	if ((x >= SSD1306_LCDWIDTH) || (y >=SSD1306_LCDHEIGHT)) return 0;
	return buffer[x+ (y/8)*SSD1306_LCDWIDTH] & (1 << y%8) ? 1 : 0;
}

/**************************************************************************/
/*! 
    @brief Clears the screen
 */
/**************************************************************************/
void ssd1306ClearScreen(void) 
{
	memset(buffer, 0, 1024);
}

/**************************************************************************/
/*! 
    @brief Renders the contents of the pixel buffer on the LCD
 */
/**************************************************************************/
void ssd1306Refresh(void) 
{
	CMD(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
	CMD(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
	CMD(SSD1306_SETSTARTLINE | 0x0); // line #0

	unsigned int i;
	for (i=0; i<1024; i++)
	{
		DATA(buffer[i]);
	}
}

/**************************************************************************/
/*!
    @brief  Draws a string using the supplied font data.

    @param[in]  x
                Starting x co-ordinate
    @param[in]  y
                Starting y co-ordinate
    @param[in]  text
                The string to render
    @param[in]  font
                Pointer to the FONT_DEF to use when drawing the string

    @section Example

    @code 

    #include "drivers/lcd/bitmap/ssd1306/ssd1306.h"
    #include "drivers/lcd/smallfonts.h"

    // Configure the pins and initialise the LCD screen
    ssd1306Init();

    // Render some text on the screen
    ssd1306DrawString(1, 10, "5x8 System", Font_System5x8);
    ssd1306DrawString(1, 20, "7x8 System", Font_System7x8);

    // Refresh the screen to see the results
    ssd1306Refresh();

    @endcode
 */
/**************************************************************************/
void ssd1306DrawString(unsigned int x, unsigned int y, const char *text, const FONT_DEF *font)
{
	unsigned char l;
	for (l = 0; l < strlen(text); l++)
	{
		ssd1306DrawChar(x + (l * (font->u8Width + 1)), y, text[l], font);
	}
}

void ssd1306DrawStringAtLine(unsigned int x, unsigned int line, const char *text, const FONT_DEF *font)
{
    ssd1306DrawString(x, line * LINE_SPACING + HEAD_MARGIN, text, font);
}

/**************************************************************************/
//Print a text and value.
void ssd1306PrintInt(unsigned int x, unsigned int y, const char *text, unsigned int val, const FONT_DEF *font)
{
	char str[150];
	unsigned char nb_char;

	nb_char = strlen(text);
    sprintf(str, "%d      ", val);
    ssd1306DrawString(x, y, text, font);
    ssd1306DrawString(x + ((nb_char + 1) * font->u8Width), y, str, font);
}
void ssd1306Printf(int x, int y, const FONT_DEF *font, const char *format, ...)
{
	char temp_buffer[43];
	va_list va_args;

	va_start(va_args, format);
	vsnprintf(temp_buffer, 43, format, va_args);
	va_end(va_args);

	ssd1306DrawString(x,y,(char *)temp_buffer, font);
}

void ssd1306PrintfAtLine(int x, int line, const FONT_DEF *font, const char *format, ...)
{
    int y = line * LINE_SPACING + HEAD_MARGIN;
    char temp_buffer[43];
    va_list va_args;

    va_start(va_args, format);
    vsnprintf(temp_buffer, 43, format, va_args);
    va_end(va_args);

    ssd1306DrawString(x, y, (char *) temp_buffer, font);
}

/**************************************************************************/
/*!
    @brief  Shifts the contents of the frame buffer up the specified
            number of pixels

    @param[in]  height
                The number of pixels to shift the frame buffer up, leaving
                a blank space at the bottom of the frame buffer x pixels
                high

    @section Example

    @code 

    #include "drivers/lcd/bitmap/ssd1306/ssd1306.h"
    #include "drivers/lcd/smallfonts.h"

    // Configure the pins and initialise the LCD screen
    ssd1306Init();

    // Enable the backlight
    ssd1306BLEnable();

    // Continually write some text, scrolling upward one line each time
    while (1)
    {
      // Shift the buffer up 8 pixels (adjust for font-height)
      ssd1306ShiftFrameBuffer(8);
      // Render some text on the screen with different fonts
      ssd1306DrawString(1, 56, "INSERT TEXT HERE", Font_System3x6);   // 3x6 is UPPER CASE only
      // Refresh the screen to see the results
      ssd1306Refresh();    
      // Wait a bit before writing the next line
      systickDelay(1000);
    }

    @endcode
 */
/**************************************************************************/
void ssd1306ShiftFrameBuffer( unsigned char height )
{
	if (height == 0) return;
	if (height >= SSD1306_LCDHEIGHT)
	{
		// Clear the entire frame buffer
		ssd1306ClearScreen();
		return;
	}

	// This is horribly inefficient, but at least easy to understand
	// In a production environment, this should be significantly optimised

	unsigned char y, x;
	for (y = 0; y < SSD1306_LCDHEIGHT; y++)
	{
		for (x = 0; x < SSD1306_LCDWIDTH; x++)
		{
			if ((SSD1306_LCDHEIGHT - 1) - y > height)
			{
				// Shift height from further ahead in the buffer
				ssd1306GetPixel(x, y + height) ? ssd1306DrawPixel(x, y) : ssd1306ClearPixel(x, y);
			}
			else
			{
				// Clear the entire line
				ssd1306ClearPixel(x, y);
			}
		}
	}
}
/**************************************************************************/
void ssd1306DrawBmp(const unsigned char *bitmap, unsigned char x, unsigned char y, unsigned char w, unsigned char h)
{
    unsigned char i;
    unsigned char j;

    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            if (bitmap[i + (j / 8) * w] & 1 << (j % 8))
            {
                ssd1306DrawPixel(x + i, y + j);
            }
        }
    }
}
/**************************************************************************/
//draw a circle outline
void ssd1306DrawCircle(unsigned char x0, unsigned char y0, unsigned char r)
{
	signed char f = 1 - r;
	signed char ddF_x = 1;
	signed char ddF_y = -2 * r;
	signed char x = 0;
	signed char y = r;
	ssd1306DrawPixel(x0, y0+r);
	ssd1306DrawPixel(x0, y0-r);
	ssd1306DrawPixel(x0+r, y0);
	ssd1306DrawPixel(x0-r, y0);

	while (x<y)
	{
		if (f>= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ssd1306DrawPixel(x0 + x, y0 + y);
		ssd1306DrawPixel(x0 - x, y0 + y);
		ssd1306DrawPixel(x0 + x, y0 - y);
		ssd1306DrawPixel(x0 - x, y0 - y);

		ssd1306DrawPixel(x0 + y, y0 + x);
		ssd1306DrawPixel(x0 - y, y0 + x);
		ssd1306DrawPixel(x0 + y, y0 - x);
		ssd1306DrawPixel(x0 - y, y0 - x);
	}
}
/**************************************************************************/
void ssd1306FillCircle(unsigned char x0, unsigned char y0, unsigned char r)
{
	int i;
	signed char f = 1 - r;
	signed char ddF_x = 1;
	signed char ddF_y = -2 * r;
	signed char x = 0;
	signed char y = r;

	for (i=y0-r; i<=y0+r; i++)
	{
		ssd1306DrawPixel(x0, i);
	}

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		for (i=y0-y; i<=y0+y; i++)
		{
			ssd1306DrawPixel(x0+x, i);
			ssd1306DrawPixel(x0-x, i);
		}
		for (i=y0-x; i<=y0+x; i++)
		{
			ssd1306DrawPixel(x0+y, i);
			ssd1306DrawPixel(x0-y, i);
		}
	}
}
/**************************************************************************/
void ssd1306ClearCircle(unsigned char x0, unsigned char y0, unsigned char r)
{
	int i;
	signed char f = 1 - r;
	signed char ddF_x = 1;
	signed char ddF_y = -2 * r;
	signed char x = 0;
	signed char y = r;

	for (i=y0-r; i<=y0+r; i++)
	{
		ssd1306ClearPixel(x0, i);
	}

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		for (i=y0-y; i<=y0+y; i++)
		{
			ssd1306ClearPixel(x0+x, i);
			ssd1306ClearPixel(x0-x, i);
		}
		for (i=y0-x; i<=y0+x; i++)
		{
			ssd1306ClearPixel(x0+y, i);
			ssd1306ClearPixel(x0-y, i);
		}
	}
}
/**************************************************************************/
// filled rectangle
void ssd1306FillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
	int i, j;
	// stupidest version - just pixels - but fast with internal buffer!
	for (i=x; i<x+w; i++) {
		for (j=y; j<y+h; j++) {
			ssd1306DrawPixel(i, j);
		}
	}
}

/**************************************************************************/
// Inverts area
void ssd1306InvertArea(unsigned char x, unsigned char y, unsigned char w, unsigned char h)
{
    int i, j;
    // stupidest version - just pixels - but fast with internal buffer!
    for (i = x; i < (x + w); i++)
    {
        for (j = y; j < (y + h); j++)
        {
            ssd1306InvertPixel(i, j);
        }
    }
}

/**************************************************************************/
void ssd1306ClearRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
	int i, j;
	// stupidest version - just pixels - but fast with internal buffer!
	for (i=x; i<x+w; i++) {
		for (j=y; j<y+h; j++) {
			ssd1306ClearPixel(i, j);
		}
	}
}
/**************************************************************************/
// draw a rectangle
void ssd1306DrawRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
	int i;
	// stupidest version - just pixels - but fast with internal buffer!
	for (i=x; i<x+w; i++) {
		ssd1306DrawPixel(i, y);
		ssd1306DrawPixel(i, y+h-1);
	}
	for (i=y; i<y+h; i++) {
		ssd1306DrawPixel(x, i);
		ssd1306DrawPixel(x+w-1, i);
	}
}

void ssd1306DrawDashedLine(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1)
{
	uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	uint8_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int8_t err = dx / 2;
	int8_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;}

	for (; x0<x1; x0++) {
		if (steep)
		{
			if ((x0 % 2) == 0)
				ssd1306DrawPixel(y0, x0);
		} else
		{
			if ((x0 % 2) == 0)
				ssd1306DrawPixel(x0, y0);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

/**************************************************************************/
// bresenham's algorithm - thx wikpedia
void ssd1306DrawLine(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1)
{
  uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  uint8_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int8_t err = dx / 2;
  int8_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;}

  for (; x0<x1; x0++) {
    if (steep) {
    	ssd1306DrawPixel(y0, x0);
    } else {
    	ssd1306DrawPixel(x0, y0);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void ssd1306ProgressBar(unsigned char x, unsigned char y, unsigned char state)
{
	ssd1306DrawRect(x, y, 104, 8);
	if (state > 0 || state <= 100)
		ssd1306FillRect(x + 2, y + 2, state, 4);

}



