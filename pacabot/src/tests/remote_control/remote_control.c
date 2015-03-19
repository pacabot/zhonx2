/* remote_control.c */

#include "config/basetypes.h"
#include "config/errors.h"

#include "hal/hal_os.h"
#include "hal/hal_serial.h"

#include "drivers/step_motor_driver.h"

HAL_SERIAL_HANDLE serial;

int test_remote_control(void)
{
    int             rv;
    unsigned char   readChar;
    unsigned int    nbytes = 1;
    unsigned char   port = 0;
    unsigned char   direction = 0; // 1 = avant, 2 = arriere, 3 = gauche, 4 = droite

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

    printf("TEST DE CONTROL A DISTANCE\n");
    printf("O   : Avancer\n");
    printf("L   : Reculer\n");
    printf("K   : Gauche\n");
    printf("M   : Droite\n");
    printf("ESC : Sortir\n");

    step_motors_init();

    hal_os_sleep(500);

    do
    {
        rv = hal_serial_read(serial, &readChar, &nbytes, FOREVER);
        switch (readChar)
        {
            case 'O':
            case 'o':
                // Avancer
                printf("Avancer\n");
                if (direction == 1)
                {
                    step_motors_move(100, 0, CHAIN_BEFORE | CHAIN_AFTER);
                }
                else
                {
                    step_motors_move(100, 0, CHAIN_AFTER);
                }
                direction = 1;
                break;
            case 'L':
            case 'l':
                // Reculer
                printf("Reculer\n");
                if (direction == 2)
                {
                    step_motors_move(-100, 0, CHAIN_BEFORE | CHAIN_AFTER);
                }
                else
                {
                    step_motors_move(-100, 0, CHAIN_AFTER);
                }
                direction = 2;
                break;
            case 'K':
            case 'k':
                // Gauche
                printf("Gauche\n");
                step_motors_rotate_in_place(45);
                direction = 3;
                break;
            case 'M':
            case 'm':
                // Droite
                printf("Droite\n");
                step_motors_rotate_in_place(-45);
                direction = 4;
                break;
            default:
                break;
        }
    }
    while (readChar != 27);

    printf("\n");
    printf("You typed 'ESC' The program is now Finished\n");

    return rv;
}
