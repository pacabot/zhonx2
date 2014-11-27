/*---------------------------------------------------------------------------
 *
 *      hal_us_sensor.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __HAL_ZIGBIT_H__
#define __HAL_ZIGBIT_H__

/* Module Identifier */
#define HAL_ZIGBIT_MODULE_ID    15

/* Error codes */
#define HAL_ZIGBIT_E_SUCCESS        0
#define HAL_ZIGBIT_E_ERROR          MAKE_ERROR(HAL_ZIGBIT_MODULE_ID, 1)
#define HAL_ZIGBIT_E_BAD_HANDLE     MAKE_ERROR(HAL_ZIGBIT_MODULE_ID, 2)

/* HAL Zigbit handle */
typedef void *HAL_ZIGBIT_HANDLE;

/* Exported functions */
int hal_zigbit_init(void);
int hal_zigbit_terminate(void);
int hal_zigbit_open(void *params, HAL_ZIGBIT_HANDLE *handle);
int hal_zigbit_close(HAL_ZIGBIT_HANDLE handle);

int hal_zigbit_send(HAL_ZIGBIT_HANDLE handle, char *data, int length);
int hal_zigbit_receive(HAL_ZIGBIT_HANDLE handle, unsigned char *data,
                       unsigned int *length, unsigned long timeoutms);

#endif // __HAL_ZIGBIT_H__
