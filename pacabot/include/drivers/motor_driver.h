/*---------------------------------------------------------------------------
 *
 *      motor_driver.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __MOTOR_DRIVER_H__
#define __MOTOR_DRIVER_H__

/* Module Identifier */
#define MOTOR_DRIVER_MODULE_ID  9

/* Error codes */
#define MOTOR_DRIVER_E_SUCCESS  0
#define MOTOR_DRIVER_E_ERROR    MAKE_ERROR(MOTOR_DRIVER_MODULE_ID, 1)

/* Motor definitions */
#define MOTOR_DIR_FWD   1   // Forward direction
#define MOTOR_DIR_BKW   0   // Backward direction

/* Exported functions */
int motors_init(void);
int motor_set_duty(unsigned char motor, unsigned char duty);
int motor_set_status(unsigned char motor, bool state);
int motor_set_direction(unsigned char motor, unsigned char direction);
int motor_get_direction(unsigned char motor, unsigned char *direction);
int motor_get_counter(unsigned char motor, unsigned long *value);
int motor_get_speed(unsigned char motor, unsigned long *value);

#endif // __MOTOR_DRIVER_H__
