/*
 * menu_colin.c
 *
 *  Created on: 4 déc. 2014
 *      Author: colin
 */
#include <stdio.h>
#include <string.h>
#include "config/basetypes.h"
#include "hal/hal_os.h"
#include "hal/hal_adc.h"
#include "hal/hal_beeper.h"
#include "hal/hal_step_motor.h"
#include "hal/hal_pid.h"
#include "hal/hal_serial.h"
#include "hal/hal_sensor.h"
#include "hal/hal_led.h"
#include "hal/hal_nvm.h"
#include "hal/hal_ui.h"

#include "app/menu_colin.h"
#include "hal/hal_ui.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"

#include "config/basetypes.h"
#include "config/errors.h"
#include "config/config.h"
#include "hal/hal_os.h"
#include "app/app_def.h"

#include "oled/arrows_bmp.h"
#include "oled/ssd1306.h"
#include "oled/smallfonts.h"
#include "app/solver_maze.h"

#define JOYSTICK_LEFT       GPIOC, GPIO_Pin_8
#define JOYSTICK_UP         GPIOC, GPIO_Pin_9
#define JOYSTICK_DOWN       GPIOC, GPIO_Pin_10
#define JOYSTICK_RIGHT      GPIOC, GPIO_Pin_11
#define JOYSTICK_PUSH       GPIOC, GPIO_Pin_12
#define RETURN_BUTTON       GPIOC, GPIO_Pin_13


extern app_config app_context;
int toto;
menuItem menu2=
{
		"menu 2" ,
		{
			{"Maze menu",'i',                       &toto },
			//{"[m]Parameters",                      &display_parameters()},
			//{"[m]Tests menu",                      &display_tests_menu()},
			{"Beeper Enabled?",'b',           &zhonx_settings.beeper_enabled},
			{NULL,                              NULL}
		}
};
menuItem menu_c =
{
		"ZHONX II                 V2.0",
		{
			{"Maze menu",'i',                       &toto },
			//{"[m]Parameters",                      &display_parameters()},
			//{"[m]Tests menu",                      &display_tests_menu()},
			{"test",'m', &menu2},
			{"Beeper Enabled?",'b',           &zhonx_settings.beeper_enabled},
			{NULL,                              NULL}
		}
};
int menu_colin(menuItem menu)
{
	unsigned char line_screen=1;
	unsigned char line_menu=0;
	hal_ui_clear_scr(app_context.ui);
	hal_ui_display_txt(app_context.ui,0,0,menu.name);
		ssd1306InvertArea(0, 10, 128, 10);
	while (true)
	{
		// Exit Button
		if (GPIO_ReadInputDataBit(JOYSTICK_LEFT) == Bit_RESET)
		{
			// Wait until button is released
			anti_rebonds (JOYSTICK_LEFT);
			return 1;
		}

		// Joystick down
		if (GPIO_ReadInputDataBit(JOYSTICK_DOWN) == Bit_RESET)
		{
			if(menu.line[line_menu+1].name!=null)
			{
				line_menu++;
				line_screen++;
				if(line_screen>6)
				{
					line_screen--;
					affiche_menu(menu,line_menu-6);
				}
			}
			anti_rebonds (JOYSTICK_DOWN);
			menu_animate((line_screen+1)*10, (line_screen+2)*10);
		}
		// Joystick up
		if (GPIO_ReadInputDataBit(JOYSTICK_UP) == Bit_RESET)
		{
			anti_rebonds (JOYSTICK_UP);

		}

		// Validate button
		if(GPIO_ReadInputDataBit(JOYSTICK_RIGHT) == Bit_RESET)
		{
			anti_rebonds (JOYSTICK_RIGHT);
		}
	}
	return -1;
}
void anti_rebonds (GPIO_TypeDef* gpio, uint16_t gpio_pin)
{
	hal_os_sleep(300);
//	unsigned long int time_base = hal_os_get_systicks();
//	do
//	{
//		if (GPIO_ReadInputDataBit(JOYSTICK_LEFT) == Bit_SET)
//			time_base = hal_os_get_systicks();
//	}while (time_base<(hal_os_get_systicks()+100));
}
void menu_animate(unsigned char y, unsigned char max_y)
{
    if (max_y > y)
    {
        for ( ; y <= max_y; y++)
        {
            ssd1306InvertArea(0, y - 1, 128, 10);
            ssd1306InvertArea(0, y, 128, 10);
            ssd1306Refresh();
            hal_os_sleep(2);
        }
    }
    else
    {
        for ( ; y >= max_y; y--)
        {
            ssd1306InvertArea(0, y + 1, 128, 10);
            ssd1306InvertArea(0, y, 128, 10);
            ssd1306Refresh();
            hal_os_sleep(2);
        }
    }
}
void affiche_menu(menuItem menu,int first_line)
{
	hal_ui_clear_scr(app_context.ui);
	hal_ui_display_txt(app_context.ui,0,0,menu.name);
	for (int i=first_line;i<first_line+6;i++)
	{
		if(menu.line[i].name!=null)
			hal_ui_display_txt(app_context.ui,0,10*i+10,menu.line[i].name);
			switch (menu.line[i].type)
			{
				case 'b':
					if(*((bool*)menu.line[i].param)==true)
						hal_ui_display_txt(app_context.ui,100,10*i+10,"yes");
					else
						hal_ui_display_txt(app_context.ui,100,10*i+10,"no");
					break;
				case 'i':
					ssd1306PrintInt(100,10*i+10," ",*((unsigned int*)menu.line[i].param),&Font_3x6);
					break;
				case 'm':
					hal_ui_display_txt(app_context.ui,117,i*10+10,">");
					break;
			}
	}
}
