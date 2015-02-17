/**
 * step_motor_driver.c
 */

/* General declarations */
#include "config/basetypes.h"
#include "config/config.h"
#include "config/errors.h"

/* HAL declarations */
#include "hal/hal_pid.h"
#include "hal/hal_adc.h"
#include "hal/hal_beeper.h"
#include "hal/hal_led.h"
#include "hal/hal_nvm.h"
#include "hal/hal_os.h"
#include "hal/hal_sensor.h"
#include "hal/hal_serial.h"
#include "hal/hal_step_motor.h"
#include "hal/hal_ui.h"

#include "oled/ssd1306.h"
#include "oled/smallfonts.h"

/* Declarations for this module */
#include "drivers/step_motor_driver.h"

/* STM32 library declarations */
#include "stm32f4xx.h"

/* App definitions */
#include "app/app_def.h"

#include <stdio.h>
#include <math.h>

#define MAX_MOTORS      2

#define EMERGENCY_STOP  1
#define DOOR_DETECTED   2

#define L_CORRECTION    1
#define R_CORRECTION    2
#define NO_CORRECTION   3

#define L_WALL_CORRECTION     1
#define R_WALL_CORRECTION     2

/* Macros */

/* Left wall is too close */
#define LEFT_WALL_CLOSE     (check_bit(sensors_val, SENSOR_L5_POS) == false)
/* Right wall is too close */
#define RIGHT_WALL_CLOSE    (check_bit(sensors_val, SENSOR_R5_POS) == false)

/* Left wall is physically present */
#define LEFT_WALL_HERE      (check_bit(sensors_val, SENSOR_L10_POS) == false)
/* Right wall is physically present */
#define RIGHT_WALL_HERE     (check_bit(sensors_val, SENSOR_R10_POS) == false)

/* Front wall is too close */
#define FRONT_WALL_CLOSE    (check_bit(sensors_val, SENSOR_F5_POS) == false)

/* Detection of left door activated */
#define DETECT_LEFT_DOOR    (check_bit(sensor_condition, SENSOR_L10_POS) == true)
/* Detection of right door activated */
#define DETECT_RIGHT_DOOR   (check_bit(sensor_condition, SENSOR_R10_POS) == true)


/* Static functions */
static int step_motors_accelerate(long distance,
                                   unsigned char sensor_condition,
                                   bool correction, double curve_coeff);
static int step_motors_decelerate(long distance,
                                   unsigned char sensor_condition,
                                   bool correction, double curve_coeff);
static int step_motors_maintain(long distance,
                                 unsigned char sensor_condition,
                                 bool correction, double curve_coeff);
static int step_motors_correction(void);
static void step_motors_stop(void);

typedef struct
{
    HAL_STEP_MOTOR_HANDLE    motor;
} step_motor;

static step_motor           motors[MAX_MOTORS];


int step_motors_init(void)
{
    int     rv;

    // Initialize Stepper motor 1
    rv = hal_step_motor_open(&(motors[0].motor), null);
    if (rv != HAL_STEP_MOTOR_E_SUCCESS)
    {
        return rv;
    }

    // Initialize Stepper motor 2
    rv = hal_step_motor_open(&(motors[1].motor), null);
    if (rv != HAL_STEP_MOTOR_E_SUCCESS)
    {
        return rv;
    }

    hal_step_motor_disable();

    // Set stepper motors precision
    rv = hal_step_motor_set_resolution(STEPPER_MOTOR_PRECISION);
    if (rv != HAL_STEP_MOTOR_E_SUCCESS)
    {
        return rv;
    }

    // Initialize direction
    hal_step_motor_set_direction(motors[0].motor, DIRECTION_FWD);
    hal_step_motor_set_direction(motors[1].motor, DIRECTION_FWD);

    return STEP_MOTOR_DRIVER_E_SUCCESS;
}


int step_motors_move(int distance_mm,
                     unsigned char sensor_condition, unsigned char chain)
{
    long    distance_steps;
    bool    accel = false;
    bool    decel = false;
    bool    maintain = false;
    bool    correction = true;
    int     rv;

    /* Process the direction */
    if (distance_mm == 0)
    {
        return STEP_MOTOR_DRIVER_E_SUCCESS;
    }

    if (distance_mm < 0)
    {
        hal_step_motor_set_direction(motors[0].motor, DIRECTION_BKW);
        hal_step_motor_set_direction(motors[1].motor, DIRECTION_BKW);
        distance_mm *= -1;
    }
    else
    {
        hal_step_motor_set_direction(motors[0].motor, DIRECTION_FWD);
        hal_step_motor_set_direction(motors[1].motor, DIRECTION_FWD);
    }

    hal_step_motor_set_acceleration(motors[0].motor, zhonx_settings.default_accel);
    hal_step_motor_set_acceleration(motors[1].motor, zhonx_settings.default_accel);

    /* Compute the total distance in steps */
    distance_steps = lround(distance_mm * STEPS_PER_MM) * 2;

    /* Do not accelerate if this is a chained command */
    accel = (!check_bit(chain, CHAIN_BEFORE));
    /* Do not decelerate if another command is to be chained to this one */
    decel = (!check_bit(chain, CHAIN_AFTER));
    maintain = (distance_steps > (zhonx_settings.max_speed_distance * 2)) ? true : false;

    /* Compute the acceleration distance */
    if (true == maintain)
    {
        if (true == accel)
        {
            rv = step_motors_accelerate(zhonx_settings.max_speed_distance,
                                        sensor_condition, correction, 1);
            if ((rv == DOOR_DETECTED) || (rv == EMERGENCY_STOP))
            {
                goto out;
            }
            distance_steps -= zhonx_settings.max_speed_distance;
        }

        if (true == decel)
        {
            distance_steps -= zhonx_settings.max_speed_distance;
            rv = step_motors_maintain(distance_steps, sensor_condition, correction, 1);
            if ((rv == DOOR_DETECTED) || (rv == EMERGENCY_STOP))
            {
                goto out;
            }
            rv = step_motors_decelerate(zhonx_settings.max_speed_distance,
                                        sensor_condition, correction, 1);
            if ((rv == DOOR_DETECTED) || (rv == EMERGENCY_STOP))
            {
                goto out;
            }
            hal_step_motor_set_freq(motors[0].motor, 0);
            hal_step_motor_set_freq(motors[1].motor, 0);
        }
        else
        {
            rv = step_motors_maintain(distance_steps, sensor_condition, correction, 1);
            if ((rv == DOOR_DETECTED) || (rv == EMERGENCY_STOP))
            {
                goto out;
            }
        }
    }
    else
    {
        if (true == accel)
        {
            distance_steps = (distance_steps / 2);
            rv = step_motors_accelerate(distance_steps, sensor_condition, correction, 1);
            if ((rv == DOOR_DETECTED) || (rv == EMERGENCY_STOP))
            {
                goto out;
            }
        }

        /* Do not decelerate if a new command is to be chained to this one */
        if (true == decel)
        {
            rv = step_motors_decelerate(distance_steps, sensor_condition, correction, 1);
            if ((rv == DOOR_DETECTED) || (rv == EMERGENCY_STOP))
            {
                goto out;
            }
            hal_step_motor_set_freq(motors[0].motor, 0);
            hal_step_motor_set_freq(motors[1].motor, 0);
        }
        else
        {
            rv = step_motors_maintain(distance_steps, sensor_condition, correction, 1);
            if ((rv == DOOR_DETECTED) || (rv == EMERGENCY_STOP))
            {
                goto out;
            }
        }
    }

out:
    if (rv != STEP_MOTOR_DRIVER_E_SUCCESS)
    {
        step_motors_stop();
    }
    return STEP_MOTOR_DRIVER_E_SUCCESS;
}


int step_motors_basic_move(int distance_mm)
{
    long    cnt_end1;
    long    cnt_end2;
    long    cnt1;
    long    cnt2;
    long    distance_steps;

    /* Process the direction */
    if (distance_mm == 0)
    {
        return STEP_MOTOR_DRIVER_E_SUCCESS;
    }

    if (distance_mm < 0)
    {
        hal_step_motor_set_direction(motors[0].motor, DIRECTION_BKW);
        hal_step_motor_set_direction(motors[1].motor, DIRECTION_BKW);
        distance_mm *= -1;
    }
    else
    {
        hal_step_motor_set_direction(motors[0].motor, DIRECTION_FWD);
        hal_step_motor_set_direction(motors[1].motor, DIRECTION_FWD);
    }

    hal_step_motor_set_acceleration(motors[0].motor, zhonx_settings.default_accel);
    hal_step_motor_set_acceleration(motors[1].motor, zhonx_settings.default_accel);

    /* Compute the total distance in steps */
    distance_steps = lround(distance_mm * STEPS_PER_MM) * 2;

    /* Set the initial speed */
    hal_step_motor_set_freq(motors[0].motor, zhonx_settings.initial_speed);
    hal_step_motor_set_freq(motors[1].motor, zhonx_settings.initial_speed);

    cnt_end1 = hal_step_motor_get_counter(motors[0].motor) + distance_steps;
    cnt_end2 = hal_step_motor_get_counter(motors[1].motor) + distance_steps;

    do
    {
        cnt1 = hal_step_motor_get_counter(motors[0].motor);
        cnt2 = hal_step_motor_get_counter(motors[1].motor);
    }
    while ((cnt1 < (cnt_end1)) || (cnt2 < (cnt_end2)));

    hal_step_motor_set_freq(motors[0].motor, 0);
    hal_step_motor_set_freq(motors[1].motor, 0);

    return STEP_MOTOR_DRIVER_E_SUCCESS;
}


int step_motors_rotate(double angle, double radius, unsigned char chain)
{
    long                    distance_steps_out;
    long                    cnt;
    long                    cnt_end;
    unsigned char           state = STATE_CURVE_ROTATE_R;
    double                  curve_rad;
    HAL_STEP_MOTOR_HANDLE   h_in;
    HAL_STEP_MOTOR_HANDLE   h_out;
    long                    freq_bak;

    /* Process the direction */
    if (angle == 0)
    {
        return STEP_MOTOR_DRIVER_E_SUCCESS;
    }

    if (angle < 0)
    {
        state = STATE_CURVE_ROTATE_L;
        angle *= -1;
        h_in  = motors[1].motor;
        h_out = motors[0].motor;
    }
    else
    {
        h_in  = motors[0].motor;
        h_out = motors[1].motor;
    }

    /* Save the frequency of inner wheel */
    freq_bak = hal_step_motor_get_freq(h_in);

    /* Get the current counter value of outer wheel */
    cnt_end = hal_step_motor_get_counter(h_out);

    /* Compute the curvature radius */
    curve_rad = radius;//(WHEELS_DISTANCE / 2);

    /* Compute the total distance in steps for the outer wheel */
    distance_steps_out = (((angle / 360.0) *
                           (curve_rad + (WHEELS_DISTANCE / 2))) / WHEEL_DIAMETER) *
                           (STEPS_PER_REVOLUTION) * 4;
    cnt_end += distance_steps_out;

    /* Set current state to 'curve rotation' */
    hal_step_motor_curve_rotate(h_in, curve_rad, state);
    do
    {
        cnt = hal_step_motor_get_counter(h_out);
    }
    while (cnt < cnt_end);

    hal_step_motor_maintain(h_in);
    hal_step_motor_maintain(h_out);

    /* Restore the frequency of the inner motor */
    hal_step_motor_set_freq(h_in, freq_bak);

    return STEP_MOTOR_DRIVER_E_SUCCESS;
}

int step_motors_curve_rotate(double angle, double radius, unsigned char chain)
{
    long                    distance_steps_out;
    long                    cnt;
    long                    cnt_end;
    int						rotate_dir;
    double					coeff_steps_in;
    double                  curve_rad;
    bool   		 			maintain = false;

    /* Set new acceleration values for the current movement */
    hal_step_motor_set_acceleration(motors[0].motor, zhonx_settings.rotate_accel);
    hal_step_motor_set_acceleration(motors[1].motor, zhonx_settings.rotate_accel);

    /* Process the direction */
    if (angle == 0)
    {
        return STEP_MOTOR_DRIVER_E_SUCCESS;
    }

    hal_step_motor_set_direction(motors[0].motor, DIRECTION_FWD);
    hal_step_motor_set_direction(motors[1].motor, DIRECTION_FWD);

    if (angle < 0)
    {
    	rotate_dir = -1;
        angle *= -1;
    }
    else
    {
    	rotate_dir = 1;
    }

    /* Compute the curvature radius */
    curve_rad = radius;//(WHEELS_DISTANCE / 2);

    /* Compute the total distance in steps for the outer wheel */
    distance_steps_out = (((angle / 360.0) *
                           (curve_rad + (WHEELS_DISTANCE / 2))) / WHEEL_DIAMETER) *
                           (STEPS_PER_REVOLUTION) * 4;


    coeff_steps_in = (((2*curve_rad) - WHEELS_DISTANCE) /
    						((2*curve_rad) + WHEELS_DISTANCE));

    coeff_steps_in *= rotate_dir;

    maintain = (distance_steps_out > (zhonx_settings.max_speed_distance * 2)) ? true : false;

    if (true == maintain)
    {
        // Accelerate
        step_motors_accelerate(zhonx_settings.max_speed_distance, 0, false, coeff_steps_in);
        distance_steps_out -= zhonx_settings.max_speed_distance;

        // Maintain speed
        distance_steps_out -= zhonx_settings.max_speed_distance;
        step_motors_maintain(distance_steps_out, 0, false, coeff_steps_in);

        // Decelerate
        step_motors_decelerate(zhonx_settings.max_speed_distance, 0, false, coeff_steps_in);
    }
    else
    {
        // Accelerate
        distance_steps_out /= 2;
        step_motors_accelerate(distance_steps_out, 0, false, coeff_steps_in);

        // Decelerate
        step_motors_decelerate(distance_steps_out, 0, false, coeff_steps_in);
    }

    // Stop motors
    hal_step_motor_set_freq(motors[0].motor, 0);
    hal_step_motor_set_freq(motors[1].motor, 0);

    return STEP_MOTOR_DRIVER_E_SUCCESS;
}

int step_motors_rotate_in_place(double angle)
{
    long    distance_steps;
    bool    maintain = false;

    /* Set new acceleration values for the current movement */
    hal_step_motor_set_acceleration(motors[0].motor, zhonx_settings.rotate_accel);
    hal_step_motor_set_acceleration(motors[1].motor, zhonx_settings.rotate_accel);

    /* Process the direction */
    if (angle == 0)
    {
        return STEP_MOTOR_DRIVER_E_SUCCESS;
    }

    if (angle < 0)
    {
        hal_step_motor_set_direction(motors[0].motor, DIRECTION_BKW);
        hal_step_motor_set_direction(motors[1].motor, DIRECTION_FWD);
        angle *= -1;
    }
    else
    {
        hal_step_motor_set_direction(motors[0].motor, DIRECTION_FWD);
        hal_step_motor_set_direction(motors[1].motor, DIRECTION_BKW);
    }

    /* Apply the correction factor */
    //angle *= ANGLE_CORRECTION;

    /* Compute the total distance in steps */
    distance_steps = lround((STEPS_PER_REVOLUTION * 2.0) *
                            (angle / 360.0) *
                            (WHEELS_DISTANCE / WHEEL_DIAMETER));

    maintain = (distance_steps > (zhonx_settings.max_speed_distance * 2)) ? true : false;

    if (true == maintain)
    {
        // Accelerate
        step_motors_accelerate(zhonx_settings.max_speed_distance, 0, false, 1);
        distance_steps -= zhonx_settings.max_speed_distance;

        // Maintain speed
        distance_steps -= zhonx_settings.max_speed_distance;
        step_motors_maintain(distance_steps, 0, false, 1);

        // Decelerate
        step_motors_decelerate(zhonx_settings.max_speed_distance, 0, false, 1);
    }
    else
    {
        // Accelerate
        distance_steps /= 2;
        step_motors_accelerate(distance_steps, 0, false, 1);

        // Decelerate
        step_motors_decelerate(distance_steps, 0, false, 1);
    }

    // Stop motors
    hal_step_motor_set_freq(motors[0].motor, 0);
    hal_step_motor_set_freq(motors[1].motor, 0);

    return STEP_MOTOR_DRIVER_E_SUCCESS;
}


/* Emergency stop */
void step_motors_stop(void)
{
    long freq_right;
    long freq_left;

    /* Set the deceleration value to emergency deceleration */
    hal_step_motor_set_acceleration(motors[0].motor, zhonx_settings.emergency_decel);
    hal_step_motor_set_acceleration(motors[1].motor, zhonx_settings.emergency_decel);

    /* Give the motors the order to decelerate */
    hal_step_motor_decelerate(motors[0].motor);
    hal_step_motor_decelerate(motors[1].motor);

    /* Wait until the motors can be stopped */
    do
    {
        /* Retrieve the frequencies of both motors */
        freq_right = hal_step_motor_get_freq(motors[0].motor);
        freq_left  = hal_step_motor_get_freq(motors[1].motor);
    }
    while ((freq_right > zhonx_settings.initial_speed) || (freq_left > zhonx_settings.initial_speed));

    /* Reset the deceleration flag */
    hal_step_motor_maintain(motors[0].motor);
    hal_step_motor_maintain(motors[1].motor);

    /* Now the motors are in the secure speed zone, so we can set their
     * frequencies to 0 securely */
    hal_step_motor_set_freq(motors[0].motor, 0);
    hal_step_motor_set_freq(motors[1].motor, 0);

}


int step_motors_accelerate(long distance,
                           unsigned char sensor_condition, bool correction, double curve_coeff)
{
    long cnt_end1;
    long cnt_end2;
    long cnt1;
    long cnt2;
    long freq;
    unsigned char sensors_val;
    int rv = STEP_MOTOR_DRIVER_E_SUCCESS;

    /* Set the initial speed */
    hal_step_motor_set_freq(motors[0].motor, zhonx_settings.initial_speed);
    hal_step_motor_set_freq(motors[1].motor, zhonx_settings.initial_speed);

    if (curve_coeff < 1)
    {
    	if (curve_coeff < 0)
    	{
            curve_coeff *= -1;
            freq = hal_step_motor_get_freq(motors[0].motor);
            hal_step_motor_set_freq(motors[0].motor,
                                   lround(freq * curve_coeff));
            cnt_end1 = hal_step_motor_get_counter(motors[0].motor) + lround(distance * curve_coeff);
            cnt_end2 = hal_step_motor_get_counter(motors[1].motor) + distance;
    	}
    	else
    	{
    		freq = hal_step_motor_get_freq(motors[1].motor);
    		hal_step_motor_set_freq(motors[1].motor,
                                lround(freq * curve_coeff));
    	    cnt_end1 = hal_step_motor_get_counter(motors[0].motor) + distance;
            cnt_end2 = hal_step_motor_get_counter(motors[1].motor) + lround(distance * curve_coeff);
    	}
	}
    else
    {
        cnt_end1 = hal_step_motor_get_counter(motors[0].motor) + distance;
        cnt_end2 = hal_step_motor_get_counter(motors[1].motor) + distance;
    }

    hal_step_motor_accelerate(motors[0].motor);
    hal_step_motor_accelerate(motors[1].motor);

    do
    {
        cnt1 = hal_step_motor_get_counter(motors[0].motor);
        cnt2 = hal_step_motor_get_counter(motors[1].motor);

        if (correction == true)
        {
            rv = step_motors_correction();
            if (rv == EMERGENCY_STOP)
            {
                return rv;
            }
        }

        if (cnt1 >= (cnt_end1))
        {
            hal_step_motor_maintain(motors[0].motor);
        }
        if (cnt2 >= (cnt_end2))
        {
            hal_step_motor_maintain(motors[1].motor);
        }

        if (((cnt_end1 - cnt1) <= CELL_LENGTH_STEP) && (sensor_condition != 0))
        {
            /* There is a sensor condition for this movement */
            sensors_val = hal_sensor_get_state(app_context.sensors);

            if (((DETECT_LEFT_DOOR == true) && (LEFT_WALL_HERE == false)) ||
                ((DETECT_RIGHT_DOOR == true) && (RIGHT_WALL_HERE == false)))
            {
                return DOOR_DETECTED;
            }
        }
    }
    while ((cnt1 < (cnt_end1)) || (cnt2 < (cnt_end2)));

    return STEP_MOTOR_DRIVER_E_SUCCESS;
}


int step_motors_decelerate(long distance,
                           unsigned char sensor_condition, bool correction, double curve_coeff)
{
    long            cnt_end1;
    long            cnt_end2;
    long            cnt1;
    long            cnt2;
    long			freq;
    int             rv = STEP_MOTOR_DRIVER_E_SUCCESS;
    unsigned char   sensors_val;
#ifdef LCD_DEBUG_MODE
    char            display_str[40];
#endif // LCD_DEBUG_MODE

    if (curve_coeff < 1)
    {
    	if (curve_coeff < 0)
    	{
            curve_coeff *= -1;
            cnt_end1 = hal_step_motor_get_counter(motors[0].motor) + lround(distance * curve_coeff);
            cnt_end2 = hal_step_motor_get_counter(motors[1].motor) + distance;

    	}
    	else
    	{
            cnt_end1 = hal_step_motor_get_counter(motors[0].motor) + distance;
            cnt_end2 = hal_step_motor_get_counter(motors[1].motor) + lround(distance * curve_coeff);
    	}
	}
    else
    {
        cnt_end1 = hal_step_motor_get_counter(motors[0].motor) + distance;
        cnt_end2 = hal_step_motor_get_counter(motors[1].motor) + distance;
    }

    hal_step_motor_decelerate(motors[0].motor);
    hal_step_motor_decelerate(motors[1].motor);

    do
    {
        cnt1 = hal_step_motor_get_counter(motors[0].motor);
        cnt2 = hal_step_motor_get_counter(motors[1].motor);

        if (cnt1 >= (cnt_end1))
        {
            hal_step_motor_maintain(motors[0].motor);
        }
        if (cnt2 >= (cnt_end2))
        {
            hal_step_motor_maintain(motors[1].motor);
        }

        if (correction == true)
        {
            rv = step_motors_correction();
            if (rv == EMERGENCY_STOP)
            {
                return rv;
            }
        }

        if (((cnt_end1 - cnt1) <= CELL_LENGTH_STEP) && (sensor_condition != 0))
        {
            /* There is a sensor condition for this movement */
            sensors_val = hal_sensor_get_state(app_context.sensors);

            if (((DETECT_LEFT_DOOR == true) && (LEFT_WALL_HERE == false)) ||
                ((DETECT_RIGHT_DOOR == true) && (RIGHT_WALL_HERE == false)))
            {
                return DOOR_DETECTED;
            }
        }
    }
    while ((cnt1 < cnt_end1) || (cnt2 < cnt_end2));


    /* Reset the deceleration flag */
//    hal_step_motor_maintain(motors[0].motor);
//    hal_step_motor_maintain(motors[1].motor);

//    if (rv == EMERGENCY_STOP)
//    {
//      /* Restore the initial acceleration values */
//      hal_step_motor_set_acceleration(motors[0].motor, zhonx_settings.default_accel);
//      hal_step_motor_set_acceleration(motors[1].motor, zhonx_settings.default_accel);

#ifdef LCD_DEBUG_MODE
        hal_ui_clear_scr(app_context.ui);
        hal_ui_display_txt(app_context.ui, 10, 1, "Nb Pas effectues:");
        printf("Debug(Nb Pas effectues:)");
        sprintf(display_str, "moteur 1: %li", hal_step_motor_get_counter(motors[0].motor) - cnt1);
        hal_ui_display_txt(app_context.ui, 10, 10, display_str);
        printf("Debug(%s)", display_str);
        sprintf(display_str, "moteur 2: %li", hal_step_motor_get_counter(motors[1].motor) - cnt2);
        hal_ui_display_txt(app_context.ui, 10, 19, display_str);
        printf("Debug(%s)", display_str);
        hal_ui_display_txt(app_context.ui, 10, 37, "RECALAGE !!!");
        printf("Debug(RECALAGE !!!)");
#endif // LCD_DEBUG_MODE
//    }

    return rv;
}


int step_motors_maintain(long distance,
                         unsigned char sensor_condition, bool correction, double curve_coeff)
{
    unsigned char   sensors_val;
    long            cnt_end1;
    long            cnt_end2;
    long            cnt1;
    long            cnt2;
    long			freq;
    int             rv = STEP_MOTOR_DRIVER_E_SUCCESS;

    if (curve_coeff < 1)
    {
    	if (curve_coeff < 0)
    	{
            curve_coeff *= -1;
            cnt_end1 = hal_step_motor_get_counter(motors[0].motor) + lround(distance * curve_coeff);
            cnt_end2 = hal_step_motor_get_counter(motors[1].motor) + distance;

    	}
    	else
    	{
            cnt_end1 = hal_step_motor_get_counter(motors[0].motor) + distance;
            cnt_end2 = hal_step_motor_get_counter(motors[1].motor) + lround(distance * curve_coeff);
    	}
	}
    else
    {
        cnt_end1 = hal_step_motor_get_counter(motors[0].motor) + distance;
        cnt_end2 = hal_step_motor_get_counter(motors[1].motor) + distance;
    }

    do
    {
        cnt1 = hal_step_motor_get_counter(motors[0].motor);
        cnt2 = hal_step_motor_get_counter(motors[1].motor);

        if (correction == true)
        {
            rv = step_motors_correction();
            if ((rv == EMERGENCY_STOP) || (rv == DOOR_DETECTED))
            {
                break;
            }
        }

        if (((cnt_end1 - cnt1) <= CELL_LENGTH_STEP) && (sensor_condition != 0))
        {
            /* There is a sensor condition for this movement */
            sensors_val = hal_sensor_get_state(app_context.sensors);

            if (((DETECT_LEFT_DOOR == true) && (LEFT_WALL_HERE == false)) ||
                ((DETECT_RIGHT_DOOR == true) && (RIGHT_WALL_HERE == false)))
            {
                hal_step_motor_set_acceleration(motors[0].motor, zhonx_settings.emergency_decel);
                hal_step_motor_set_acceleration(motors[1].motor, zhonx_settings.emergency_decel);
                rv = DOOR_DETECTED;
                break;
            }
        }
    }
    while ((cnt1 < (cnt_end1)) || (cnt2 < (cnt_end2)));

    return rv;
}

int step_motors_correction(void)
{
    unsigned char         sensors_val;
    long                  freq_right = 0;
    long                  freq_left  = 0;
    volatile long         freq_correction;
    static unsigned char  i = 0;
    static unsigned char  j = 0;
    static int			  correction_loop = 0;

//    correction_loop++;

//    if (correction_loop != 10000)
//    {
//    	return 0;
//    }

//    correction_loop = 0;

    sensors_val = hal_sensor_get_state(app_context.sensors);

    if (LEFT_WALL_HERE == true)
    {
        if ((hal_pid_get_wall_correction() != L_WALL_CORRECTION) || (i < 2))
        {
            j = 0;
            i++;
            hal_pid_left_set_count(zhonx_settings.max_correction / i);
            hal_pid_right_set_count(zhonx_settings.max_correction / i);
            hal_pid_set_type_correction(NO_CORRECTION);
            hal_pid_set_wall_correction(L_WALL_CORRECTION);
        }
        /* Left wall is physically present */
        if (LEFT_WALL_CLOSE == true)
        {
            if (hal_pid_get_type_correction() != R_CORRECTION)                                // test if is a new type correction
            {
                hal_pid_left_set_old_count(hal_pid_left_get_count());                         // Update last count correction
                hal_pid_left_set_count(0);                                                    // Reset last count correction
                hal_pid_set_type_correction(R_CORRECTION);                                    // Update new type correction
            }

            /* Left wall is too close */
            hal_led_set_state(app_context.led, HAL_LED_COLOR_RED, DISABLE);
            hal_led_set_state(app_context.led, HAL_LED_COLOR_ORANGE, ENABLE);

            /* Retrieve the frequencies of both motors */
            freq_right = hal_step_motor_get_freq(motors[0].motor);
            freq_left  = hal_step_motor_get_freq(motors[1].motor);

            freq_correction = zhonx_settings.correction_p + hal_pid_left_get_old_count();

            hal_step_motor_set_freq(motors[0].motor,
                                    lround((freq_right + freq_left) / 2) - freq_correction);
            hal_step_motor_set_freq(motors[1].motor,
                                    lround((freq_right + freq_left) / 2) + freq_correction);
        }
        else
        {
            if (hal_pid_get_type_correction() != L_CORRECTION)                              // test if is a new type correction
            {
                hal_pid_right_set_old_count(hal_pid_right_get_count());                     // Update last count correction
                hal_pid_right_set_count(0);                                                 // Reset last count correction
                hal_pid_set_type_correction(L_CORRECTION);                                  // Update new type correction
            }

            hal_led_set_state(app_context.led, HAL_LED_COLOR_ORANGE, DISABLE);
            hal_led_set_state(app_context.led, HAL_LED_COLOR_RED, ENABLE);

            /* Retrieve the frequencies of both motors */
            freq_right = hal_step_motor_get_freq(motors[0].motor);
            freq_left  = hal_step_motor_get_freq(motors[1].motor);

            //freq_correction = lround(((freq_right + freq_left) / 2) / lround((zhonx_settings.correction_p - hal_pid_right_get_old_count())/10));
            freq_correction = zhonx_settings.correction_p + hal_pid_right_get_old_count();

            hal_step_motor_set_freq(motors[0].motor,
                                    lround((freq_right + freq_left) / 2) + freq_correction);
            hal_step_motor_set_freq(motors[1].motor,
                                    lround((freq_right + freq_left) / 2) - freq_correction);
        }
    }

    else if (RIGHT_WALL_HERE == true)
    {
        if ((hal_pid_get_wall_correction() != R_WALL_CORRECTION) || (j < 2))
        {
            i = 0;
            j++;
            hal_pid_left_set_count(zhonx_settings.max_correction / j);
            hal_pid_right_set_count(zhonx_settings.max_correction / j);
            hal_pid_set_type_correction(NO_CORRECTION);
            hal_pid_set_wall_correction(R_WALL_CORRECTION);
        }
        /* Right wall is physically present */
        if (RIGHT_WALL_CLOSE == true)
        {
            if (hal_pid_get_type_correction() != L_CORRECTION)                              // test if is a new type correction
            {
                hal_pid_right_set_old_count(hal_pid_right_get_count());                       // Update last count correction
                hal_pid_right_set_count(0);                                                   // Reset last count correction
                hal_pid_set_type_correction(L_CORRECTION);                                    // Update new type correction
            }

            /* Right wall is too close */
            hal_led_set_state(app_context.led, HAL_LED_COLOR_ORANGE, DISABLE);
            hal_led_set_state(app_context.led, HAL_LED_COLOR_RED, ENABLE);

            /* Retrieve the frequencies of both motors */
            freq_right = hal_step_motor_get_freq(motors[0].motor);
            freq_left  = hal_step_motor_get_freq(motors[1].motor);

            freq_correction = zhonx_settings.correction_p + hal_pid_right_get_old_count();

            hal_step_motor_set_freq(motors[0].motor,
                                    lround((freq_right + freq_left) / 2) + freq_correction);
            hal_step_motor_set_freq(motors[1].motor,
                                    lround((freq_right + freq_left) / 2) - freq_correction);
        }
        else
        {
            if (hal_pid_get_type_correction() != R_CORRECTION)                              // test if is a new type correction
            {
                hal_pid_left_set_old_count(hal_pid_left_get_count());                         // Update last count correction
                hal_pid_left_set_count(0);                                                    // Reset last count correction
                hal_pid_set_type_correction(R_CORRECTION);                                    // Update new type correction
            }

            hal_led_set_state(app_context.led, HAL_LED_COLOR_RED, DISABLE);
            hal_led_set_state(app_context.led, HAL_LED_COLOR_ORANGE, ENABLE);

            /* Retrieve the frequencies of both motors */
            freq_right = hal_step_motor_get_freq(motors[0].motor);
            freq_left  = hal_step_motor_get_freq(motors[1].motor);

            //freq_correction = lround(((freq_right + freq_left) / 2) /
            freq_correction = zhonx_settings.correction_p + hal_pid_left_get_old_count();

            hal_step_motor_set_freq(motors[0].motor,
                                    lround((freq_right + freq_left) / 2) - freq_correction);
            hal_step_motor_set_freq(motors[1].motor,
                                    lround((freq_right + freq_left) / 2) + freq_correction);
        }
    }
    else
    {
        hal_led_set_state(app_context.led, HAL_LED_COLOR_RED, DISABLE);
        hal_led_set_state(app_context.led, HAL_LED_COLOR_ORANGE, DISABLE);

        /* Retrieve the frequencies of both motors */
        freq_right = hal_step_motor_get_freq(motors[0].motor);
        freq_left  = hal_step_motor_get_freq(motors[1].motor);

        freq_correction = ((freq_right + freq_left) / 2) / zhonx_settings.correction_p;

        hal_step_motor_set_freq(motors[0].motor, (freq_right + freq_left)/2);
        hal_step_motor_set_freq(motors[1].motor, (freq_right + freq_left)/2);

        hal_pid_set_type_correction(NO_CORRECTION);
    }

    if (FRONT_WALL_CLOSE)
    {
        /* The front wall is too close */
        return EMERGENCY_STOP;
    }

    //    ssd1306ClearScreen();
    //    ssd1306PrintInt(5, 10, "L_cnt = ", hal_pid_left_get_count(), Font_System7x8);
    //    ssd1306PrintInt(5, 20, "R_cnt = ", hal_pid_right_get_count(), Font_System7x8);
    //    ssd1306PrintInt(5, 30, "old_L = ", hal_pid_left_get_old_count(), Font_System7x8);
    //    ssd1306PrintInt(5, 40, "old_R = ", hal_pid_right_get_old_count(), Font_System7x8);
    //    ssd1306PrintInt(5, 50, "type  = ", hal_pid_get_type_correction(), Font_System7x8);
    //    ssd1306Refresh();

//    if ((hal_step_motor_get_freq(motors[0].motor) || hal_step_motor_get_freq(motors[1].motor)) < 1)
//    {
//        while (1)
//        {
//            ssd1306ClearScreen();
//            ssd1306PrintInt(5, 10, "Freq_L = ", hal_step_motor_get_freq(motors[0].motor), &Font_7x8);
//            ssd1306PrintInt(5, 20, "Freq_R = ", hal_step_motor_get_freq(motors[1].motor), &Font_7x8);
//            ssd1306Refresh();
//        }
//    }

    return 0;
}
