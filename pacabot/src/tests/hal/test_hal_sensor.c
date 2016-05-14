/* test_hal_sensor.c */

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

#include "stm32f4xx_gpio.h"
#include "stm32f4xx.h"

#include "app/app_def.h"

#include <stdio.h>


int test_hal_sensor(void)
{
    unsigned char   val = 0;
    char			str[256];

    while(1)
    {
    	hal_ui_clear_scr(app_context.ui);
        val = hal_sensor_get_state(app_context.sensors);
        sprintf(str,"5cm right:  %i\n",((val & SENSOR_R5_POS) == 0 ? 1 : 0));
        hal_ui_display_txt(app_context.ui, 10, 1, str);
        sprintf(str, "5cm front:  %i\n", ((val & SENSOR_F5_POS) == 0 ? 1 : 0));
        hal_ui_display_txt(app_context.ui, 10, 10, str);
        sprintf(str, "5cm left:   %i\n", ((val & SENSOR_L5_POS) == 0 ? 1 : 0));
        hal_ui_display_txt(app_context.ui, 10, 19, str);
        sprintf(str, "10cm right: %i\n", ((val & SENSOR_R10_POS) == 0 ? 1 : 0));
        hal_ui_display_txt(app_context.ui, 10, 28, str);
        sprintf(str, "10cm front: %i\n", ((val & SENSOR_F10_POS) == 0 ? 1 : 0));
        hal_ui_display_txt(app_context.ui, 10, 37, str);
        sprintf(str, "10cm left:  %i\n", ((val & SENSOR_L10_POS) == 0 ? 1 : 0));
        hal_ui_display_txt(app_context.ui, 10, 46, str);
        hal_ui_refresh(app_context.ui);
        HAL_Delay(50);

		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) != Bit_SET)
		{
		    while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) != Bit_SET);
			break;
		}
    }

    return 0;
}
