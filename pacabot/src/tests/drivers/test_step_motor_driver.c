/* test_step_motor_driver.c */

#include "config/config.h"
#include "config/basetypes.h"
#include "config/errors.h"

#include "hal/hal_os.h"
#include "hal/hal_step_motor.h"
#include "hal/hal_sensor.h"

#include "drivers/step_motor_driver.h"

#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"

HAL_SENSOR_HANDLE sensors;

int test_step_motor_driver(void)
{
    int     rv;

    rv = hal_sensor_init();
    if (rv != HAL_SENSOR_E_SUCCESS)
    {
        return rv;
    }

    rv = hal_sensor_open(&sensors, null);
    if (rv != HAL_SENSOR_E_SUCCESS)
    {
        return rv;
    }

    hal_step_motor_enable();

    HAL_Delay(500);

    step_motors_move(1500, 0, 0);
    step_motors_rotate_in_place(180);
    step_motors_move(1500, 0, 0);

    HAL_Delay(1000);
    hal_step_motor_disable();

    return 0;
}
