/*---------------------------------------------------------------------------
 *
 *      hal_zigbit.c
 *
 *---------------------------------------------------------------------------*/

/* General declarations */
#include "config/basetypes.h"
#include "config/config.h"
#include "config/errors.h"

#include <string.h>

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

/* Declarations for this module */
#include "hal/hal_zigbit.h"
#include "hal/hal_serial.h"

typedef struct {
    HAL_SERIAL_HANDLE com;
} hal_zigbit_handle;

static hal_zigbit_handle zigbit;


int hal_zigbit_init(void)
{
    unsigned char port = 1;
    int           rv;

    // Open USART 2 interface
    rv = hal_serial_open(&(zigbit.com), &port, 38400);
    if (rv != HAL_SERIAL_E_SUCCESS)
    {
        return rv;
    }
    return HAL_ZIGBIT_E_SUCCESS;
}


int hal_zigbit_terminate(void)
{
    return HAL_ZIGBIT_E_SUCCESS;
}


int hal_zigbit_open(void *params, HAL_ZIGBIT_HANDLE *handle)
{
    UNUSED(params);
    if (handle == NULL)
    {
        return HAL_ZIGBIT_E_BAD_HANDLE;
    }
    *handle = &zigbit;

    return HAL_ZIGBIT_E_SUCCESS;
}


int hal_zigbit_close(HAL_ZIGBIT_HANDLE handle)
{
    hal_zigbit_handle *h;

    if (handle == NULL)
    {
        return HAL_ZIGBIT_E_BAD_HANDLE;
    }
    h = (hal_zigbit_handle *)handle;

    return hal_serial_close(h->com);
}


int hal_zigbit_send(HAL_ZIGBIT_HANDLE handle, char *data, int length)
{
    hal_zigbit_handle *h;

    if (handle == NULL)
    {
        return HAL_ZIGBIT_E_BAD_HANDLE;
    }
    h = (hal_zigbit_handle *)handle;

    return hal_serial_write(h->com, data, length);
}


int hal_zigbit_receive(HAL_ZIGBIT_HANDLE handle, unsigned char *data,
                       unsigned int *length, unsigned long timeoutms)
{
    hal_zigbit_handle *h;

    if (handle == NULL)
    {
        return HAL_ZIGBIT_E_BAD_HANDLE;
    }
    h = (hal_zigbit_handle *)handle;

    return hal_serial_read(h->com, data, length, FOREVER);
}
