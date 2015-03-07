/* test_hal_led.c */

#include "config/basetypes.h"
#include "config/errors.h"

#include "hal/hal_os.h"
#include "hal/hal_led.h"

#include "stm32f4xx.h"

HAL_LED_HANDLE leds;


int test_hal_led(void)
{
//    int           rv;

//    rv = hal_led_init();
//    if (rv != HAL_LED_E_SUCCESS)
//    {
//        return rv;
//    }
//    rv = hal_led_open(&leds, (void *)0);
//    if (rv != HAL_LED_E_SUCCESS)
//    {
//        return rv;
//    }

    for (int i=0;i<5;i++)
    {
        hal_led_set_state(leds, HAL_LED_COLOR_ORANGE, ENABLE);
        hal_led_set_state(leds, HAL_LED_COLOR_RED, ENABLE);

        hal_os_sleep(500);

        hal_led_set_state(leds, HAL_LED_COLOR_ORANGE, DISABLE);
        hal_led_set_state(leds, HAL_LED_COLOR_RED, DISABLE);
    }
    return 0;
}
