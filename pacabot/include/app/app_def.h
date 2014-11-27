/* app_def.h */

#ifndef __APP_DEF_H__
#define __APP_DEF_H__

typedef struct
{
	/* Handles to HAL modules */
    HAL_ADC_HANDLE          adc;
    HAL_BEEPER_HANDLE       beeper;
    HAL_LED_HANDLE          led;
    HAL_NVM_HANDLE          nvm;
    HAL_SERIAL_HANDLE 	    serial;
	HAL_SENSOR_HANDLE       sensors;
	HAL_UI_HANDLE           ui;
} app_config;

extern app_config app_context;

#endif // __APP_DEF_H__
