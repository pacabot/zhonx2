/* test_hal_beeper.c */

#include "config/basetypes.h"
#include "config/errors.h"

#include "hal/hal_os.h"
#include "hal/hal_beeper.h"

#include "stm32f4xx.h"

HAL_BEEPER_HANDLE beeper;


int test_hal_beeper(void)
{
    int           rv;
    unsigned int  i;
    long          freq = 1000;
    long          duration;

    rv = hal_beeper_init();
    if (rv != HAL_BEEPER_E_SUCCESS)
    {
        return rv;
    }

    rv = hal_beeper_open(&beeper, (void *)0);
    if (rv != HAL_BEEPER_E_SUCCESS)
    {
        return rv;
    }

    for (i = 0; i < 4; i++)
    {
        while (freq < 1800)
        {
            hal_beeper_beep(beeper, freq, 25);
            HAL_Delay(15);
            freq += 100;
        }
        while (freq > 800)
        {
            hal_beeper_beep(beeper, freq, 25);
            HAL_Delay(15);
            freq -= 100;
        }
    }

    HAL_Delay(500);

    i = 0;
    freq = 660;
    duration = 200;

    while (i < 4)
    {
        if (i == 3)
        {
            freq = 2640;
            duration = 500;
        }
        hal_beeper_beep(beeper, freq, duration);
        HAL_Delay(1000 - duration);
        i++;
    }
	HAL_Delay(1000);
	hal_beeper_beep(beeper, 1046, 400);
	hal_beeper_beep(beeper, 1320, 320);
	hal_beeper_beep(beeper, 1568, 240);
	hal_beeper_beep(beeper, 2092, 800);

    return 0;
}
