/**
 * motor_driver.c
 */

/* General declarations */
#include "config/basetypes.h"
#include "config/config.h"
#include "config/errors.h"

/* HAL declarations */
#include "hal/hal_pwm.h"
#include "hal/hal_motor_encoder.h"

/* Declarations for this module */
#include "drivers/motor_driver.h"

/* STM32 library declarations */
#include "stm32f4xx.h"

#define MAX_MOTORS  2
#define IS_VALID_MOTOR(MOT) ((((MOT) >= 0) && ((MOT) <= (MAX_MOTORS))) ? true : false)
#define IS_VALID_DIRECTION(DIR) (((DIR) == MOTOR_DIR_FWD) || ((DIR) == MOTOR_DIR_BKW))

typedef struct {
    HAL_PWM_HANDLE           pwmA;
    HAL_PWM_HANDLE           pwmB;
    HAL_MOTOR_ENCODER_HANDLE encoder;
    unsigned char            direction;
    unsigned char            duty;
} motor;

static motor motors[MAX_MOTORS];


/**
 * @brief Initializes the motor drivers and encoders
 */
int motors_init(void)
{
    hal_pwm_params           pwm_params;
    hal_motor_encoder_params encoder_params;
    int                      rv;

    // Initialize HAL modules
    rv = hal_pwm_init();
    if (rv != HAL_PWM_E_SUCCESS)
    {
        return rv;
    }

    rv = hal_motor_encoder_init();
    if (rv != HAL_MOTOR_ENCODER_E_SUCCESS)
    {
        return rv;
    }

    // Initialize direction
    motors[0].direction = MOTOR_DIR_FWD;
    motors[1].direction = MOTOR_DIR_FWD;

    // Initialize duty
    motors[0].duty = 0;
    motors[1].duty = 0;

    // Initialize motor 1 - Channel 1
    pwm_params.channel = 0;

    // Initialize PWM
    rv = hal_pwm_open(&(motors[0].pwmA), &pwm_params);
    if (rv != HAL_PWM_E_SUCCESS)
    {
        return rv;
    }

    // Initialize motor 1 - Channel 2
    pwm_params.channel++;

    rv = hal_pwm_open(&(motors[0].pwmB), &pwm_params);
    if (rv != HAL_PWM_E_SUCCESS)
    {
        return rv;
    }

    // Initialize Incremental encoder
    encoder_params.channel = 0;

    rv = hal_motor_encoder_open(&(motors[0].encoder), &encoder_params);
    if (rv != HAL_MOTOR_ENCODER_E_SUCCESS)
    {
        return rv;
    }

    // Initialize motor 2 - channel 1
    pwm_params.channel++;

    // Initialize PWM
    rv = hal_pwm_open(&(motors[1].pwmA), &pwm_params);
    if (rv != HAL_PWM_E_SUCCESS)
    {
        return rv;
    }

    // Initialize motor 2 - channel 2
    pwm_params.channel++;

    // Initialize PWM
    rv = hal_pwm_open(&(motors[1].pwmB), &pwm_params);
    if (rv != HAL_PWM_E_SUCCESS)
    {
        return rv;
    }

    // Initialize Incremental encoder
    encoder_params.channel++;

    rv = hal_motor_encoder_open(&(motors[1].encoder), &encoder_params);
    if (rv != HAL_MOTOR_ENCODER_E_SUCCESS)
    {
        return rv;
    }

    return MOTOR_DRIVER_E_SUCCESS;
}


/**
 * @brief Sets the duty of a motor
 *
 * @param[in] motor Motor number
 * @param[in] duty  The desired duty
 *
 * @retval MOTOR_DRIVER_E_SUCCESS if operation was successful
 */
int motor_set_duty(unsigned char motor, unsigned char duty)
{
    int rv;

    if (false == IS_VALID_MOTOR(motor))
    {
        return MOTOR_DRIVER_E_ERROR;
    }

    switch (motors[motor].direction)
    {
        case MOTOR_DIR_FWD:
            rv  = hal_pwm_set_duty(motors[motor].pwmA, duty);
            rv |= hal_pwm_set_duty(motors[motor].pwmB, 0);
            break;
        case MOTOR_DIR_BKW:
            rv  = hal_pwm_set_duty(motors[motor].pwmA, 0);
            rv |= hal_pwm_set_duty(motors[motor].pwmB, duty);
            break;
        default:
            return MOTOR_DRIVER_E_ERROR;
    }

    motors[motor].duty = duty;

    return rv;
}


/**
 * @brief Enables or disables a motor
 *
 * @param[in] motor Motor number
 * @param[in] state Desired state
 *
 * @retval MOTOR_DRIVER_E_SUCCESS if operation was successful
 */
int motor_set_state(unsigned char motor, bool state)
{
    if (false == IS_VALID_MOTOR(motor))
    {
        return MOTOR_DRIVER_E_ERROR;
    }

    switch (state)
    {
        case true:
            hal_pwm_enable(motors[motor].pwmA);
            hal_pwm_enable(motors[motor].pwmB);
            break;
        default:
            hal_pwm_disable(motors[motor].pwmA);
            hal_pwm_disable(motors[motor].pwmB);
    }

    return MOTOR_DRIVER_E_SUCCESS;
}


/**
 * @brief Sets the direction of a motor
 *
 * @param[in] motor     Motor number
 * @param[in] direction Desired direction
 *            This parameter can take one of the following values:
 *              #MOTOR_DIR_FWD  The motor will move forward
 *              #MOTOR_DIR_BKW  The motor will move backward
 *
 * @retval MOTOR_DRIVER_E_SUCCESS if operation was successful
 */
int motor_set_direction(unsigned char motor, unsigned char direction)
{
    if (false == IS_VALID_MOTOR(motor))
    {
        return MOTOR_DRIVER_E_ERROR;
    }

    if (false == IS_VALID_DIRECTION(direction))
    {
        return MOTOR_DRIVER_E_ERROR;
    }

    motors[motor].direction = direction;

    return motor_set_duty(motor, motors[motor].duty);
}


/**
 * @brief Gets the direction of a motor
 *
 * @param[in]       motor     Motor number
 * @param[in/out]   direction Pointer toward the direction
 *
 * @retval MOTOR_DRIVER_E_SUCCESS if operation was successful
 */
int motor_get_direction(unsigned char motor, unsigned char *direction)
{
    if (false == IS_VALID_MOTOR(motor))
    {
        return MOTOR_DRIVER_E_ERROR;
    }
    *direction = motors[motor].direction;

    return MOTOR_DRIVER_E_SUCCESS;
}


/**
 * @brief Gets the incremental encoder value of a motor
 *
 * @param[in]       motor   Motor number
 * @param[in/out]   value   Valid pointer toward the counter value
 *
 * @retval MOTOR_DRIVER_E_SUCCESS if operation was successful
 */
int motor_get_counter(unsigned char motor, unsigned long *value)
{
    if (false == IS_VALID_MOTOR(motor))
    {
        return MOTOR_DRIVER_E_ERROR;
    }

    return hal_motor_encoder_get_count(motors[motor].encoder, value);
}


/**
 * @brief Gets the speed of a motor in counts/second
 *
 * @param[in]       motor   Motor number
 * @param[in/out]   value   Valid pointer toward the speed value
 *
 * @retval MOTOR_DRIVER_E_SUCCESS if operation was successful
 */
int motor_get_speed(unsigned char motor, unsigned long *value)
{
    if (false == IS_VALID_MOTOR(motor))
    {
        return MOTOR_DRIVER_E_ERROR;
    }

    return hal_motor_encoder_get_speed(motors[motor].encoder, value);
}
