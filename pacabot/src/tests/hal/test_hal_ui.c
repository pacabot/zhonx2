/* test_hal_ui.c */

#include "config/basetypes.h"
#include "config/errors.h"

#include "hal/hal_os.h"
#include "hal/hal_ui.h"

#include "stm32f4xx.h"

HAL_UI_HANDLE ui;


int test_hal_ui(void)
{
    int             rv;

    /* Initialize Hardware Abstraction Layer */
//    rv = hal_os_init();
//    if (rv != HAL_OS_SUCCESS)
//    {
//            return rv;
//    }

    rv = hal_ui_init();
    if (rv != HAL_UI_E_SUCCESS)
    {
            return rv;
    }

    while(1);

    return rv;
}
