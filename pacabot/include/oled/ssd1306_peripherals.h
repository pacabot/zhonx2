/**************************************************************************/
/*!
    @file     ssd1306_peripherals.h
    @author   PLF (PACABOT)
    @date     16 December 2012
    @version  0.10
 */
/**************************************************************************/
#ifndef __SSD1306_PERIPHERALS_H__
#define __SSD1306_PERIPHERALS_H__

#define HARDWARE_SPI_MACRO				// If this line is commented you will use the software SPI

// Pin Definitions
#define RST_PIN                  		GPIO_Pin_10
#define RST_GPIO_PORT             		GPIOB
#define RST_GPIO_CLK              		RCC_AHB1Periph_GPIOB

#define DC_PIN                   		GPIO_Pin_11
#define DC_GPIO_PORT             		GPIOB
#define DC_GPIO_CLK              		RCC_AHB1Periph_GPIOB

#define CS_PIN                   		GPIO_Pin_12
#define CS_GPIO_PORT             		GPIOB
#define CS_GPIO_CLK              		RCC_AHB1Periph_GPIOB

/* SPIx Communication boards Interface */
#define SPIx                           SPI2
#define SPIx_CLK                       RCC_APB1Periph_SPI2
#define SPIx_CLK_INIT                  RCC_APB1PeriphClockCmd

#define SPIx_SCK_PIN                   GPIO_Pin_13
#define SPIx_SCK_GPIO_PORT             GPIOB
#define SPIx_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define SPIx_SCK_SOURCE                GPIO_PinSource13
#define SPIx_SCK_AF                    GPIO_AF_SPI1

#define SPIx_MOSI_PIN                  GPIO_Pin_15
#define SPIx_MOSI_GPIO_PORT            GPIOB
#define SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define SPIx_MOSI_SOURCE               GPIO_PinSource15
#define SPIx_MOSI_AF                    GPIO_AF_SPI1


void ssd1306_GPIO_Config(void);
void ssd1306_SPI_Config(void);

#endif
