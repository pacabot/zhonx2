/* test_hal_led.c */

#include "config/basetypes.h"
#include "config/errors.h"

#include "hal/hal_os.h"
#include "hal/hal_adc.h"
#include "hal/hal_sensor.h"
#include "hal/hal_serial.h"
#include "hal/hal_beeper.h"
#include "hal/hal_led.h"
#include "hal/hal_nvm.h"
#include "hal/hal_ui.h"
#include "hal/hal_step_motor.h"

#include "app/app_def.h"

#include <stdio.h>

#include "stm32f4xx_gpio.h"
#include "stm32f4xx.h"

int test_hal_color_sensor(void)
{
    int             rv;
    char            str[100];
    unsigned long   timeout;

    while(1)
    {
        hal_ui_clear_scr(app_context.ui);

        rv = hal_sensor_get_color(app_context.sensors);

        sprintf(str, "Color sens: %i\n", rv);
        hal_ui_display_txt(app_context.ui, 10, 1, str);

        hal_ui_refresh(app_context.ui);

        timeout = hal_os_get_systicks() + 500;
        do
        {
            if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) != Bit_SET)
            {
                // Wait until button is released
                while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) != Bit_SET);
                hal_os_sleep(200);
                return 0;
            }
        }
        while (timeout > hal_os_get_systicks());
    }

    return 0;
}
