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

extern void step_motors_accelerate(long distance, bool correction);
extern void step_motors_decelerate(long distance, bool correction);


int test_motor_rotate(void)
{
    hal_step_motor_enable();
    HAL_Delay(2000);

//    step_motors_move(450, 0, 0);
//    step_motors_rotate_in_place(180);
//    hal_os_sleep(2000);
//    step_motors_move(450, 0, 0);
//    step_motors_rotate_in_place(180);
//    hal_os_sleep(2000);
//    step_motors_move(450, 0, 0);
//    step_motors_rotate_in_place(180);
//    hal_os_sleep(2000);
//    step_motors_move(450, 0, 0);
//    step_motors_rotate_in_place(180);
//    step_motors_curve_rotate(-180, 90, 0);
//    hal_os_sleep(2000);
//    step_motors_curve_rotate(-180, 60, 0);
//    hal_os_sleep(2000);



    step_motors_move(90+180*0.95, 0, 0);
    step_motors_curve_rotate(-90, 88, 0);
  //  step_motors_move(, 0, 0);
    step_motors_curve_rotate(90, 88, 0);
    step_motors_move(180*0.95, 0, 0);
    step_motors_curve_rotate(90, 88, 0);
    step_motors_move(90*0.95, 0, 0);

//    step_motors_rotate_in_place(180);

    hal_step_motor_disable();

    return 0;
}

int test_hal_step_motor(void)
{
    int ii;

    hal_step_motor_enable();
    HAL_Delay(1000);

    step_motors_move(450, 0, 0);
    //hal_os_sleep(10000);
    //step_motors_move(-1000, 0, 0);
    //hal_os_sleep(1000);

    for (ii = 0; ii < 10; ii++)
    {
        step_motors_rotate_in_place(180);
        HAL_Delay(1000);
        step_motors_move(700, 0, 0);
    }

    HAL_Delay(3000);

    for (ii = 0; ii < 10; ii++)
    {
        step_motors_rotate_in_place(-180);
        HAL_Delay(1000);
        step_motors_move(700, 0, 0);
    }

    HAL_Delay(3000);

    hal_step_motor_disable();

    return 0;
}
