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
/* Extern functions */
extern int test_hal_adc(void);
extern int test_hal_color_sensor(void);
extern int test_hal_serial(void);
extern int test_hal_sensor(void);
extern int test_hal_beeper(void);
extern int test_hal_led(void);
extern int test_hal_ui(void);
//extern int test_hal_nvm(void);
extern int test_hal_step_motor(void);
extern int test_oled1(void);
extern int test_oled2(void);
extern int test_oled3(void);
extern int test_step_motor_driver(void);
extern int test_remote_control(void);
extern int test_motor_rotate(void);
extern int distance_cal(void);
extern int test_maze_trajectoire(void);
extern int maze(int i, int str);

extern int sensor_calibrate(void);
extern void run_trajectory(int trajectory_nb);


extern app_config app_context;

menuItem maze_menu =
{
		"maze menu",
		{
				{"New maze",'f',			&maze},
				{"x finish",'l',			&zhonx_settings.x_finish_maze},
				{"y finish",'l',			&zhonx_settings.y_finish_maze},
				{"color finish",'b',		&zhonx_settings.color_sensor_enabled},
				{"calibration enabled",'b',	&zhonx_settings.calibration_enabled}
//				{"Trajectory...",'f',		&trajectory_menu},
//				{"Restore maze",            &restore_maze_menu}
		}
};
menuItem motor_menu=
{
    "MOTOR MENU",
    {
		{"Random move",'f',				null},
		{"Rotate calib.",'f',			test_motor_rotate},
		{"Distance calib.",'f',			distance_cal},
		{"Stepper motor move",'f',		test_step_motor_driver},
		{"Acceleration calib.",'f',		null}
    }
};
menuItem motion_settings =
{
    "MOTION SETTINGS",
    {
		{"Initial speed :",'l',			&zhonx_settings.initial_speed},
		{"Max speed dist:",'l',			&zhonx_settings.max_speed_distance},
		{"Default accel :",'l',			&zhonx_settings.default_accel},
		{"Rotate accel  :",'l',			&zhonx_settings.rotate_accel},
		{"Emerg decel   :",'l',			&zhonx_settings.emergency_decel}
    }
};
menuItem PID_settings =
{
    "PID SETTINGS",
    {
		{"Proportional  :",'l',			&zhonx_settings.correction_p},
		{"Integral      :",'l',			&zhonx_settings.correction_i},
		{"Max correction:",'l',			&zhonx_settings.max_correction},
		{NULL,			NULL,			NULL}
    }
};
menuItem sensor_settings =
{
    "COLOR SENSOR SETTINGS",
    {
		{"[b]Enabled?      :",'b',		&zhonx_settings.color_sensor_enabled},
		{"Calibrate color",'f',			sensor_calibrate},
		{"Threshold val :",'l',			&zhonx_settings.threshold_color},
		{"End Greater?  :",'b',			&zhonx_settings.threshold_greater},
		{NULL,			NULL,			NULL}
    }
};
menuItem paramters_menu=
{
		"parameters menu",
		{
			{"Motion settings",'m',			&motion_settings},
			{"PID settings",'m',			&PID_settings},
			{"Sensor settings",'m',			&sensor_settings},
			{NULL,			NULL,			NULL}
//			{"Save settings",'m',			&save_settings},
//			{"Restore settings",'m',		&restore_settings}
		}
};
menuItem tests_menu=
{
		"test menu",
		{
				{"Test ADC",'f',				test_hal_adc},
				{"Test beeper",'f',				test_hal_beeper},
//				{"Test OLED",'m',				&oled_menu},
				{"Test motor",'m',				&motor_menu},
				{"Test sensor",'f',				test_hal_sensor},
				{"Test color sensor",'f',		test_hal_color_sensor},
				{NULL,			NULL,			NULL}
		}
};
static const menuItem oled_menu =
{
    "OLED MENU",
    {
		{"OLED Test 1",'f',				test_oled1},
		{"OLED Test 2",'f',				test_oled2},
		{"OLED Test 3",'f',				test_oled3},
		{NULL,			NULL,			NULL}
    }
};
menuItem menu_c =
{
		"ZHONX II                 V2.0",
		{
			{"Maze menu",'m',			&maze_menu },
			{"prameters",'m',			&paramters_menu},
			{"test menu",'m',			&tests_menu},
			{"beeper enabled?",'b',		&zhonx_settings.beeper_enabled},
			{NULL,			NULL,			NULL}
		}
};
int menu_colin(menuItem menu)
{
	signed char line_screen=1;
	signed char line_menu=0;
	affiche_menu(menu,line_menu);
	ssd1306InvertArea(0, 10, 128, 10);
	hal_ui_refresh(app_context.ui);
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
            hal_beeper_beep(app_context.beeper, 4000, 10);
			anti_rebonds (JOYSTICK_DOWN);
			if(menu.line[line_menu+1].name!=null)
			{
				line_menu++;
				line_screen++;
				if(line_screen>MAX_LINE_SCREEN)
				{
					line_screen--;
					affiche_menu(menu,line_menu-(line_screen-1));
					ssd1306InvertArea(0, line_screen*10, 128, 10);
					hal_ui_refresh(app_context.ui);
				}
				else
				{
					menu_animate((line_screen-1)*10+1, (line_screen)*10-1);
				}
			}
		}
		// Joystick up
		if (GPIO_ReadInputDataBit(JOYSTICK_UP) == Bit_RESET)
		{
			anti_rebonds (JOYSTICK_UP);
            hal_beeper_beep(app_context.beeper, 4000, 10);
			if(line_screen==1)
			{
				if(line_menu>0)
				{
					line_menu--;
					affiche_menu(menu,line_menu);
					ssd1306InvertArea(0, 10, 128, 10);
					hal_ui_refresh(app_context.ui);
				}
			}
			else
			{
				line_menu--;
				line_screen--;
				menu_animate((line_screen+1)*10-1, (line_screen)*10);
			}

		}

		// Validate button
		if(GPIO_ReadInputDataBit(JOYSTICK_RIGHT) == Bit_RESET)
		{

            hal_beeper_beep(app_context.beeper, 4000, 10);
			anti_rebonds (JOYSTICK_RIGHT);
			switch(menu.line[line_menu].type)
			{
				case 'b':
					hal_ui_modify_bool_param(app_context.ui,menu.line[line_menu].name,(bool*) menu.line[line_menu].param);
					break;
				case 'i':
					hal_ui_modify_int_param(app_context.ui,menu.line[line_menu].name,(int*)menu.line[line_menu].param,1);
					break;
				case 'l':
					hal_ui_modify_long_param(app_context.ui,menu.line[line_menu].name,menu.line[line_menu].param,1);
					break;
				case 'm':
					menu_colin(*(menuItem*)menu.line[line_menu].param);
					break;
				case 'f':
					(void*)menu.line[line_menu].param();
					break;
				default:
					break;
			}
			affiche_menu(menu,line_menu-(line_screen-1));
			ssd1306InvertArea(0,10*line_screen,168,10);
			hal_ui_refresh(app_context.ui);
		}
	}
	return -1;
}
void anti_rebonds (GPIO_TypeDef* gpio, uint16_t gpio_pin)
{
	unsigned long int time_base = hal_os_get_systicks();
	do
	{
		if (GPIO_ReadInputDataBit(JOYSTICK_LEFT) == Bit_RESET)
			time_base = hal_os_get_systicks();
	}while (time_base!=(hal_os_get_systicks()-200));
}
void menu_animate(unsigned char y_, unsigned char max_y_)
{
	unsigned char y=y_;
	unsigned char max_y=max_y_;

    if (max_y > y)
    {
        for ( ; y <= max_y; y++)
        {
            ssd1306InvertArea(0, y - 1, 128, 10);
            ssd1306InvertArea(0, y, 128, 10);
            ssd1306Refresh();
            hal_os_sleep(10);
        }
    }
    else
    {
        for ( ; y >= max_y; y--)
        {
            ssd1306InvertArea(0, y + 1, 128, 10);
            ssd1306InvertArea(0, y, 128, 10);
            ssd1306Refresh();
            hal_os_sleep(10);
        }
    }
}
void affiche_menu(menuItem menu,int line)
{
	hal_ui_clear_scr(app_context.ui);
	hal_ui_display_txt(app_context.ui,0,0,menu.name);
	for (int i=0;i<MAX_LINE_SCREEN;i++)
	{
		if(menu.line[i].name!=null)
			hal_ui_display_txt(app_context.ui,0,10*i+10,menu.line[line].name);
			switch (menu.line[line].type)
			{
				case 'b':
					if(*((bool*)menu.line[line].param)==true)
						hal_ui_display_txt(app_context.ui,100,10*i+10,"yes");
					else
						hal_ui_display_txt(app_context.ui,100,10*i+10,"no");
					break;
				case 'i':
					ssd1306PrintInt(100,10*i+10," ",*((unsigned int*)menu.line[line].param),&Font_3x6);
					break;
				case 'l':
					ssd1306PrintInt(100,10*i+10," ",*((unsigned long*)menu.line[line].param),&Font_3x6);
					break;
				case 'f':
				case 'm':
					hal_ui_display_txt(app_context.ui,117,i*10+10,">");
					break;
			}
		line ++;
	}
}
