/* test_hal_step_motor.c */

#include "config/config.h"
#include "config/basetypes.h"
#include "config/errors.h"

#include "hal/hal_os.h"
#include "hal/hal_step_motor.h"
#include "hal/hal_sensor.h"

#include "drivers/step_motor_driver.h"

#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"


int distance_cal(void)
{
    int ii;
    hal_step_motor_enable();
    hal_os_sleep(2000);

    for (ii = 0; ii < 4; ii++)
    {
        step_motors_move(540, SENSOR_L10_POS | SENSOR_R10_POS, 0);
        hal_os_sleep(2000);
        step_motors_rotate_in_place(180);
        hal_os_sleep(2000);
    }

//    for (ii = 0; ii < 4; ii++)
//    {
//        step_motors_move(530, 0, 0);
//        step_motors_rotate_in_place(180);
//        step_motors_move(530, 0, 0);
//        step_motors_rotate_in_place(180);
//
//        step_motors_move(180, 0, 0);
//        step_motors_move(180, 0, 0);
//        step_motors_move(180, 0, 0);
//
//        step_motors_rotate_in_place(180);
//        step_motors_move(180, 0, 0);
//        step_motors_move(180, 0, 0);
//        step_motors_move(180, 0, 0);
//        step_motors_rotate_in_place(180);
//    }

    hal_os_sleep(1000);

    hal_step_motor_disable();

    return 0;
}
