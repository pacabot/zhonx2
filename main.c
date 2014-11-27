/*---------------------------------------------------------------------------
 *
 *      main.c
 *
 *---------------------------------------------------------------------------*/

#include "config/basetypes.h"
#include "config/errors.h"
#include "config/config.h"

#include "hal/hal_os.h"
#include "hal/hal_adc.h"
#include "hal/hal_beeper.h"
#include "hal/hal_step_motor.h"
#include "hal/hal_pid.h"
#include "hal/hal_serial.h"
#include "hal/hal_sensor.h"
#include "hal/hal_led.h"
#include "hal/hal_nvm.h"
#include "hal/hal_ui.h"

#include "drivers/step_motor_driver.h"

#include "util/common_functions.h"

#include "app/app_def.h"

#include <math.h>

#include <stdio.h>

extern int display_main_menu(void);

static int app_initialization(void);

app_config app_context;


int main(void)
{
    int rv;

    rv = app_initialization();
    if (rv != 0)
    {
        return rv;
    }

    //rv = test_hal_motor_encoder();

    //rv = test_maze_trajectoire();
    rv = display_main_menu();

    //rv = test_hal_beeper();
    //rv = test_hal_step_motor();
    //rv = test_hal_beeper();
	//rv = test_hal_serial();
	//rv = test_hal_sensor();
    //rv = test_hal_led();
    //rv = test_hal_ui();
    //rv = test_oled_exemple();
    //rv = test_maze_trajectoire();
    //rv = test_step_motor_driver();

	return 0;
}


int app_initialization(void)
{
    int rv;

    zhonx_settings.initial_speed = INITIAL_SPEED;
    zhonx_settings.max_speed_distance = MAX_SPEED_DISTANCE;
    zhonx_settings.default_accel = DEFAULT_ACCEL;
    zhonx_settings.rotate_accel = ROTATE_ACCEL;
    zhonx_settings.emergency_decel = EMERGENCY_DECEL;
    zhonx_settings.correction_p = CORRECTION_P;
    zhonx_settings.correction_i = CORRECTION_I;
    zhonx_settings.max_correction = MAX_CORRECTION;
    zhonx_settings.calibration_enabled = false;
    zhonx_settings.color_sensor_enabled = true;
    zhonx_settings.threshold_color = 40000;

    rv = hal_os_init();
    if (rv != HAL_OS_SUCCESS)
    {
        return rv;
    }
//    rv = hal_serial_init();
//    if (rv != HAL_SERIAL_E_SUCCESS)
//    {
//        return rv;
//    }
    rv = hal_adc_init();
    if (rv != HAL_ADC_E_SUCCESS)
    {
        return rv;
    }
    rv = hal_beeper_init();
    if (rv != HAL_BEEPER_E_SUCCESS)
    {
        return rv;
    }
    rv = hal_led_init();
    if (rv != HAL_LED_E_SUCCESS)
    {
        return rv;
    }
    rv = hal_nvm_init();
    if (rv != HAL_NVM_E_SUCCESS)
    {
        return rv;
    }
    rv = hal_pid_init();
    if (rv != HAL_PID_E_SUCCESS)
    {
        return rv;
    }
    rv = hal_sensor_init();
    if (rv != HAL_SENSOR_E_SUCCESS)
    {
        return rv;
    }
    rv = hal_step_motor_init();
    if (rv != HAL_STEP_MOTOR_E_SUCCESS)
    {
        return rv;
    }
    rv = hal_ui_init();
    if (rv != HAL_UI_E_SUCCESS)
    {
        return rv;
    }


    rv = hal_adc_open(&app_context.adc, null);
    if (rv != HAL_ADC_E_SUCCESS)
    {
        return rv;
    }
    rv = hal_beeper_open(&app_context.beeper, null);
    if (rv != HAL_BEEPER_E_SUCCESS)
    {
        return rv;
    }
    rv = hal_led_open(&app_context.led, null);
    if (rv != HAL_LED_E_SUCCESS)
    {
        return rv;
    }
    rv = hal_nvm_open(&app_context.nvm, null);
    if (rv != HAL_NVM_E_SUCCESS)
    {
        return rv;
    }
    rv = hal_sensor_open(&app_context.sensors, null);
    if (rv != HAL_SENSOR_E_SUCCESS)
    {
        return rv;
    }
    rv = hal_ui_open(&app_context.ui, app_context.adc);
    if (rv != HAL_UI_E_SUCCESS)
    {
        return rv;
    }

    rv = step_motors_init();

    return rv;
}
