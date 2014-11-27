/*---------------------------------------------------------------------------
 *
 *      zigbit_driver.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __ZIGBIT_DRIVER_H__
#define __ZIGBIT_DRIVER_H__

/* Module Identifier */
#define ZIGBIT_DRIVER_MODULE_ID  11

/* Error codes */
#define ZIGBIT_DRIVER_E_SUCCESS  0
#define ZIGBIT_DRIVER_E_ERROR    MAKE_ERROR(ZIGBIT_DRIVER_MODULE_ID, 1)

int zigbit_driver_init(void);

#endif // __ZIGBIT_DRIVER_H__
