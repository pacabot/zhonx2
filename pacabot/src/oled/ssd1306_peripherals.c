/**************************************************************************/
/*
    @file     ssd1306.c
    @author   PLF (PACABOT)
    @date     10 December 2012
    @version  1.05

    Driver for 128x64 OLED display based on the ssd1306 controller.
 */
/**************************************************************************/

/**************************************************************************/
/* Includes                                                               */
/**************************************************************************/
#include "misc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"
#include "hal/hal_os.h"
#include "oled/ssd1306.h"
#include "oled/smallfonts.h"

#include "oled/ssd1306_peripherals.h"

/**************************************************************************/
/*  Private Function Prototypes                                           */
/**************************************************************************/

/**************************************************************************/
/*  External Functions Prototypes                                           */
/**************************************************************************/

/**************************************************************************/
/* Macros                                                                 */
/**************************************************************************/

/**************************************************************************/
/* Private Methods                                                        */
/**************************************************************************/

/**************************************************************************/
/* Public Methods                                                         */
/**************************************************************************/
//Initialise Oled GPIO
void ssd1306_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(RST_GPIO_CLK | DC_GPIO_CLK | CS_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = RST_PIN;
	GPIO_Init(RST_GPIO_PORT, &GPIO_InitStructure);

	/* SPI  MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  DC_PIN;
	GPIO_Init(DC_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  CS_PIN;
	GPIO_Init(CS_GPIO_PORT, &GPIO_InitStructure);
}

/**************************************************************************/
//Draws a single pixel
void ssd1306_SPI_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	/* Peripheral Clock Enable -------------------------------------------------*/
	/* Enable the SPI clock */
	SPIx_CLK_INIT(SPIx_CLK, ENABLE);

	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(SPIx_SCK_GPIO_CLK | SPIx_MOSI_GPIO_CLK, ENABLE);

	/* SPI GPIO Configuration --------------------------------------------------*/
	/* Connect SPI pins to AF5 */
	GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT, SPIx_SCK_SOURCE, SPIx_SCK_AF);
	GPIO_PinAFConfig(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_SOURCE, SPIx_MOSI_AF);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPIx_SCK_PIN;
	GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);

	/* SPI  MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPIx_MOSI_PIN;
	GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(SPIx);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;							// Clock Polarity ACTIVE state is High or Low
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;						// Clock Phase Data is sampled on the first or second clock edge transition
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

/*	 Configure the Priority Group to 1 bit
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	 Configure the SPI interrupt priority
	NVIC_InitStructure.NVIC_IRQChannel = SPIx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
*/
	/* Initializes the SPI communication -----------------------------------------*/
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPIx, &SPI_InitStructure);

	/* The Data transfer is performed in the SPI interrupt routine----------------*/
	/* Enable the SPI peripheral */
	SPI_Cmd(SPIx, ENABLE);
}
