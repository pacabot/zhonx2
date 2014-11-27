/*---------------------------------------------------------------------------
 *
 *      hal_pwm.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __HAL_PWM_H__
#define __HAL_PWM_H__

/* Module Identifier */
#define HAL_PWM_MODULE_ID    9

/* Error codes */
#define HAL_PWM_E_SUCCESS    0
#define HAL_PWM_E_ERROR      MAKE_ERROR(HAL_PWM_MODULE_ID, 1)
#define HAL_PWM_E_BAD_HANDLE MAKE_ERROR(HAL_PWM_MODULE_ID, 2)

typedef struct {
    unsigned char duty;
    unsigned char channel;
} hal_pwm_params;

typedef void *HAL_PWM_HANDLE;

int hal_pwm_init(void);
int hal_pwm_terminate(void);
int hal_pwm_open(HAL_PWM_HANDLE *handle, void *params);
int hal_pwm_close(HAL_PWM_HANDLE handle);
int hal_pwm_enable(HAL_PWM_HANDLE handle);
int hal_pwm_disable(HAL_PWM_HANDLE handle);
int hal_pwm_set_duty(HAL_PWM_HANDLE handle, unsigned char duty);
int hal_pwm_set_freq(HAL_PWM_HANDLE handle, unsigned long freq);


#endif /* __HAL_PWM_H__ */
