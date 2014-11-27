/*---------------------------------------------------------------------------
 *
 *      hal_motor_encoder.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __HAL_MOTOR_ENCODER_H__
#define __HAL_MOTOR_ENCODER_H__

/* Module Identifier */
#define HAL_MOTOR_ENCODER_MODULE_ID    5

/* Error codes */
#define HAL_MOTOR_ENCODER_E_SUCCESS    MAKE_ERROR(HAL_MOTOR_ENCODER_MODULE_ID, 0)
#define HAL_MOTOR_ENCODER_E_ERROR      MAKE_ERROR(HAL_MOTOR_ENCODER_MODULE_ID, 1)
#define HAL_MOTOR_ENCODER_E_BAD_HANDLE MAKE_ERROR(HAL_MOTOR_ENCODER_MODULE_ID, 2)

/* HAL MOTOR_ENCODER handle */
typedef void *HAL_MOTOR_ENCODER_HANDLE;

typedef struct {
    unsigned char channel;
} hal_motor_encoder_params;


/* Exported functions */
int hal_motor_encoder_init(void);
int hal_motor_encoder_terminate(void);
int hal_motor_encoder_open(HAL_MOTOR_ENCODER_HANDLE *handle, void *params);
int hal_motor_encoder_close(HAL_MOTOR_ENCODER_HANDLE handle);
int hal_motor_encoder_get_count(HAL_MOTOR_ENCODER_HANDLE handle, unsigned long *value);
int hal_motor_encoder_get_speed(HAL_MOTOR_ENCODER_HANDLE handle, unsigned long *value);

#endif // __HAL_MOTOR_ENCODER_H__
