/*---------------------------------------------------------------------------
 *
 *      hal_led.c
 *
 *---------------------------------------------------------------------------*/

/* Common declarations */
#include "config/config.h"
#include "config/basetypes.h"
#include "config/errors.h"

/* The declarations for this module */
#include "hal/hal_led.h"

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"


#define LEDS_PORT   GPIOA
#define LED1_PIN    GPIO_Pin_11
#define LED2_PIN    GPIO_Pin_12
#define LED1        LEDS_PORT, LED1_PIN
#define LED2        LEDS_PORT, LED2_PIN


/* Static functions */
static void RCC_Configuration(void);
static void GPIO_Configuration(void);

typedef struct
{
    unsigned char color;
} hal_led_handle;

hal_led_handle leds;


int hal_led_init(void)
{
    RCC_Configuration();
    GPIO_Configuration();
    return HAL_LED_E_SUCCESS;
}


int hal_led_terminate(void)
{
    return HAL_LED_E_SUCCESS;
}


int hal_led_open(HAL_LED_HANDLE *handle, void *param)
{
    UNUSED(param);

    *handle = &leds;

    return HAL_LED_E_SUCCESS;
}


int hal_led_close(HAL_LED_HANDLE handle)
{
    UNUSED(handle);
    
    return HAL_LED_E_SUCCESS;
}


int hal_led_toggle(HAL_LED_HANDLE handle,
                   unsigned char color,
                   unsigned long duration)
{
    UNUSED(handle);
    UNUSED(color);
    UNUSED(duration);

    return HAL_LED_E_SUCCESS;
}


int hal_led_reset(HAL_LED_HANDLE handle)
{
    UNUSED(handle);

    GPIO_ResetBits(LEDS_PORT, LED1_PIN | LED2_PIN);

    return HAL_LED_E_SUCCESS;
}


int hal_led_set_state(HAL_LED_HANDLE handle,
                      unsigned char color, unsigned char state)
{
    UNUSED(handle);

    switch (color)
    {
        case HAL_LED_COLOR_RED:
            GPIO_WriteBit(LED1, state);
            break;
        case HAL_LED_COLOR_ORANGE:
            GPIO_WriteBit(LED2, state);
            break;
    }

    return HAL_LED_E_SUCCESS;
}


/**
 * @brief  Configures the different system clocks.
 * @param  None
 * @retval None
 */
void RCC_Configuration(void)
{
    /* GPIO clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
}


/**
 * @brief  Configure the GPIO pins.
 * @param  None
 * @retval None
 */
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* LEDs configuration */
    GPIO_InitStructure.GPIO_Pin = LED1_PIN | LED2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LEDS_PORT, &GPIO_InitStructure);
}
