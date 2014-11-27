/*---------------------------------------------------------------------------
 *
 *      hal_adc.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __HAL_ADC_H__
#define __HAL_ADC_H__

/* Module Identifier */
#define HAL_ADC_MODULE_ID    1

/* Error codes */
#define HAL_ADC_E_SUCCESS    0
#define HAL_ADC_E_ERROR      MAKE_ERROR(HAL_ADC_MODULE_ID, 1)
#define HAL_ADC_E_BAD_HANDLE MAKE_ERROR(HAL_ADC_MODULE_ID, 2)

/* HAL MOTOR_ENCODER handle */
typedef void *HAL_ADC_HANDLE;


/* Exported functions */
int hal_adc_init(void);

int hal_adc_terminate(void);

int hal_adc_open(HAL_ADC_HANDLE *handle, void *params);

int hal_adc_close(HAL_ADC_HANDLE handle);

int hal_adc_get_value(HAL_ADC_HANDLE handle,
                      unsigned char channel, unsigned short *value);

#endif // __HAL_ADC_H__
