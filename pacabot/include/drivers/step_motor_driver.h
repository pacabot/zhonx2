/**
 * motor_driver.h
 */

#ifndef __STEP_MOTOR_DRIVER_H__
#define __STEP_MOTOR_DRIVER_H__

/* Module Identifier */
#define STEP_MOTOR_DRIVER_MODULE_ID  10

/* Error codes */
#define STEP_MOTOR_DRIVER_E_SUCCESS  0
#define STEP_MOTOR_DRIVER_E_ERROR    MAKE_ERROR(STEP_MOTOR_DRIVER_MODULE_ID, 1)

/* Driver definitions */
#define CHAIN_BEFORE  0x01
#define CHAIN_AFTER 0x02

/* Exported functions */

/**
 * @brief Initializes the stepper motor drivers
 */
int step_motors_init(void);


/**
 * @brief Move to a specified distance
 *
 * This function makes the robot move forward or backward
 * to the specified distance
 *
 * @param[in]   distance_mm         the distance in millimeters. If the distance
 *                                  is negative, the robot moves backward.
 * @param[in]   sensor_condition    an exit condition of the function, even if
 *                                  the trajectory was not completed.
 * @param[in]   chain               to chain the trajectory. Can be a bitwise OR of:
 *                                  @arg CHAIN_BEFORE  indicates that the acceleration
 *                                                   is not needed.
 *                                  @arg CHAIN_AFTER indicates that the deceleration
 *                                                   is not needed.
 *
 * @retval      STEP_MOTOR_DRIVER_E_SUCCESS if operation was successful
 *              Any other value otherwise.
 */
int step_motors_move(int distance_mm,
                     unsigned char sensor_condition, unsigned char chain);
int step_motors_basic_move(int distance_mm);
int step_motors_curve_rotate(double angle, double radius, unsigned char chain);
int step_motors_rotate(double angle, double radius, unsigned char chain);
int step_motors_rotate_in_place(double angle);
void step_motor_stop(void);

#endif // __STEP_MOTOR_DRIVER_H__
