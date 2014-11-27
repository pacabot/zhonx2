/* test_hal_serial.c */

#include "config/basetypes.h"
#include "config/errors.h"

#include "hal/hal_os.h"
#include "hal/hal_serial.h"

HAL_SERIAL_HANDLE serial;

int test_hal_serial(void)
{
    int             rv;
    unsigned char   readChar;
    unsigned int    nbytes = 1;
    unsigned char   port = 0;

    /* Initialize Hardware Abstraction Layer */
// //   rv = hal_os_init();
//    if (rv != HAL_OS_SUCCESS)
//    {
//        return rv;
//    }

    rv = hal_serial_open(&serial, &port, 115200);
    if (rv != HAL_SERIAL_E_SUCCESS)
    {
        return rv;
    }

    printf("Hello my name is %s and I am %d years old!", "Pacabot", 12);

    do
    {
        rv = hal_serial_read(serial, &readChar, &nbytes, FOREVER);
        rv = hal_serial_write(serial, &readChar, 1);
    }
    while (readChar != 13);

    printf("");
    printf("You typed 'ENTER' The program is now Finished");

    return rv;
}
