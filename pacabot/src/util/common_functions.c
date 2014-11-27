/*---------------------------------------------------------------------------
 *
 *      common_functions.c
 *
 *---------------------------------------------------------------------------*/

/* General declarations */
#include "config/basetypes.h"
#include "config/config.h"
#include "config/errors.h"

#include "hal/hal_os.h"
#include "hal/hal_adc.h"
#include "hal/hal_beeper.h"
#include "hal/hal_led.h"
#include "hal/hal_nvm.h"
#include "hal/hal_serial.h"
#include "hal/hal_sensor.h"
#include "hal/hal_ui.h"

/* ST Lib declarations */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"

#include <string.h>

#include "app/app_def.h"

/* Declarations for this module */
#include "util/common_functions.h"

#define TOUPPER(x) ((('a' <= (x)) && (x) <= 'z') ? (((x) - 'a') + 'A') : (x))

//extern app_config app_context;


// prend une chaine de caractère composée de chiffres Hexa et convertit en binaire
void ConvertBinaire(char tab[][MAZE_MAX_SIZE])
{
    char pos = 0;
    char caract;
    int ii,jj;
    for (ii = 0; ii < MAZE_MAX_SIZE; ii++)
    {
        for (jj = 0; jj < MAZE_MAX_SIZE; jj++)
        {
            caract = tab[ii][jj];
            if (caract!=0)
            {
                pos = caract - '0';
                if (pos > 15) pos = pos - 7;
            }
            tab[ii][jj]=pos;
        }
    }
}


//une séquence binaire est convertie en chaine pour affichage
void Convertcharacter(char tab[],char nb)
{
    char pos;
    char caract;
    int ii;
    for (ii = 0; ii<nb; ii++)
    {
       caract = tab[ii];
       pos = caract + '0';
       tab[ii]=pos;
    }
}


void bip4(void)
{
    int i = 0;

    while (i < 3)
    {
        hal_beeper_beep(app_context.beeper, 650, 200);
        hal_os_sleep(800);
        i++;
    }
    hal_beeper_beep(app_context.beeper, 2640, 500);
    hal_os_sleep(500);
}


char getHexChar(char bt)
{
    bt &= 0x0f;
    if (((signed char)bt >= 0) && (bt <= 9))
    {
        return ('0' + bt);
    }
    else
    {
        return ('A' + bt - 10);
    }
}


const char *hexdump(const void *data, unsigned int len)
{
    static char string[1024*4];
    unsigned char *d = (unsigned char *) data;
    unsigned int i;
    unsigned int j = 0;

    string[0] = '\0';
    for (i = 0; len--; i += 3)
    {
        if (i >= sizeof(string) -4)
        {
            break;
        }
        //sprintf(string+i," %02x", *d++);
        string[j++] = getHexChar((unsigned char)(*d >> 4));
        string[j++] = getHexChar((unsigned char)(*d++ & 0x0f));
        string[j++] = ' ';
    }
    string[--j] = '\0';

    return string;
}

char *touppercase(char *str, unsigned int len)
{
    static char     upper_str[256];
    unsigned int    i;

    upper_str[0] = '\0';
    /* Limit maximum length */
    if (len > 255)
    {
        len = 255;
    }

    for (i = 0; i < len; i++)
    {

        upper_str[i] = TOUPPER(str[i]);
    }
    upper_str[i] = '\0';

    return upper_str;
}
