/*---------------------------------------------------------------------------
 *
 *      hal_serial.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __HAL_SERIAL_H__
#define __HAL_SERIAL_H__

/* Module Identifier */
#define HAL_SERIAL_MODULE_ID    11

/* Error codes */
#define HAL_SERIAL_E_SUCCESS    0
#define HAL_SERIAL_E_ERROR      MAKE_ERROR(HAL_SERIAL_MODULE_ID, 1)
#define HAL_SERIAL_E_TIMEOUT    MAKE_ERROR(HAL_SERIAL_MODULE_ID, 2)
#define HAL_SERIAL_E_BAD_HANDLE MAKE_ERROR(HAL_SERIAL_MODULE_ID, 3)

/* HAL Serial handle */
typedef void *HAL_SERIAL_HANDLE;


/* Exported functions */
int hal_serial_init(void);

int hal_serial_terminate(void);

int hal_serial_open(HAL_SERIAL_HANDLE *handle, void *params, unsigned long baudrate);

int hal_serial_close(HAL_SERIAL_HANDLE handle);

int hal_serial_read(HAL_SERIAL_HANDLE handle, void *data,
					unsigned int *nbytes, unsigned long timeoutms);

int hal_serial_write(HAL_SERIAL_HANDLE handle, void *data, unsigned int nbytes);

#endif /* __HAL_SERIAL_H__ */
