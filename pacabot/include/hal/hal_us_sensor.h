/*---------------------------------------------------------------------------
 *
 *      hal_us_sensor.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __HAL_US_SENSOR_H__
#define __HAL_US_SENSOR_H__

/* Module Identifier */
#define HAL_US_SENSOR_MODULE_ID    14

/* Error codes */
#define HAL_US_SENSOR_E_SUCCESS        0
#define HAL_US_SENSOR_E_ERROR          MAKE_ERROR(HAL_US_SENSOR_MODULE_ID, 1)
#define HAL_US_SENSOR_E_BAD_HANDLE     MAKE_ERROR(HAL_US_SENSOR_MODULE_ID, 2)

/* HAL Ultrasonic Sensor handle */
typedef void *HAL_US_SENSOR_HANDLE;

/* Exported functions */
int hal_us_sensor_init(void);
int hal_us_sensor_terminate(void);
int hal_us_sensor_open(void *params, HAL_US_SENSOR_HANDLE *handle);
int hal_us_sensor_close(HAL_US_SENSOR_HANDLE handle);

#endif // __HAL_US_SENSOR_H__
