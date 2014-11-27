/*---------------------------------------------------------------------------
 *
 *      hal_pid.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __HAL_PID_H__
#define __HAL_PID_H__

/* Module Identifier */
#define HAL_PID_MODULE_ID    8

/* Error codes */
#define HAL_PID_E_SUCCESS        0
#define HAL_PID_E_ERROR          MAKE_ERROR(HAL_PID_MODULE_ID, 1)
#define HAL_PID_E_BAD_HANDLE     MAKE_ERROR(HAL_PID_MODULE_ID, 2)
#define HAL_PID_E_BAD_DIRECTION  0xFF

/* PID definitions */
/** Opaque data type definition for the Led interface */
typedef void *HAL_PID_HANDLE;

/**
 * @brief Initializes PID module
 *
 * @description This function initializes timers
 *              needed to generate Stepper Motor corrections.
 *
 * @param       None
 * @retval      #HAL_PID_E_SUCCESS if operation was successful
 */

int hal_pid_init(void);
int hal_pid_terminate(void);
int hal_pid_open(void);//HAL_PID_HANDLE *handle, void *params);
int hal_pid_close(void);

//int hal_pid_attach(int (*funcptr)(void));
void hal_pid_enable(void);
void hal_pid_disable(void);
bool hal_pid_isEnabled(void);

long hal_pid_left_get_count(void);
long hal_pid_right_get_count(void);
int  hal_pid_left_set_count(long count);
int  hal_pid_right_set_count(long count);
long hal_pid_left_get_old_count(void);
long hal_pid_right_get_old_count(void);
int  hal_pid_left_set_old_count(long count);
int  hal_pid_right_set_old_count(long count);
int  hal_pid_get_type_correction(void);
int  hal_pid_set_type_correction(int type);
int  hal_pid_get_wall_correction(void);
int  hal_pid_set_wall_correction(int type);

#endif /* __HAL_PID_H__ */
