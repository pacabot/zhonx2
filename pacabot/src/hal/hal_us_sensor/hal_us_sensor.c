/*---------------------------------------------------------------------------
 *
 *      hal_us_sensor.c
 *
 *---------------------------------------------------------------------------*/

#if 0

/* General declarations */
#include "config/basetypes.h"
#include "config/config.h"
#include "config/errors.h"

#include <string.h>

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"

/* Declarations for this module */
#include "hal/hal_us_sensor.h"

#define US_SENSORS_PORT GPIOC

#define US_SENSOR_TIMER TIM7

#ifdef STM32F100RB
/* APB2 bus frequency (in Hz) */
# define APB2_FREQ 24000000
/* PWM Timer base frequency (in Hz) */
# define TIMER_FREQ 240000

#else
/* APB2 bus frequency (in Hz) */
# define APB2_FREQ 72000000
/* Timer base frequency (in Hz) */
# define TIMER_FREQ 3200000
#endif // STM32F100RB

/* Toggle frequency (in Hz) */
#define CARRIER_FREQ 40000
/* Timer prescaler (PSC register) */
#define TIMER_PRESCALER (((APB2_FREQ) / (TIMER_FREQ)) - 1)
/* Timer period (ARR register) */
#define TIMER_PERIOD    (((TIMER_FREQ) / (CARRIER_FREQ)) - 1)

#define MAX_US_SENSORS  4
#define DEFAULT_ETU     10  // Elementary time unit

/* Static functions */
static void RCC_Configuration(void);
static void GPIO_Configuration(void);
static void NVIC_Configuration(void);

typedef struct {
    long distance;
    unsigned char etu;
} us_sensor_handle;

us_sensor_handle us_sensors[MAX_US_SENSORS];


int hal_us_sensor_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    int                     idx;

    //memset(us_sensors, 0, sizeof(us_sensor_handle));
    for (idx = 0; idx < MAX_US_SENSORS; idx++)
    {
        us_sensors[idx].distance = 0;
        us_sensors[idx].etu = DEFAULT_ETU;
    }

    /* System Clocks Configuration */
    RCC_Configuration();
    /* GPIO Configuration */
    GPIO_Configuration();
    /* Nested Vector Interrupt Controller configuration */
    NVIC_Configuration();

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = TIMER_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = TIMER_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(US_SENSOR_TIMER, &TIM_TimeBaseStructure);

    /* Enable interrupts on Update event */
    TIM_ITConfig(US_SENSOR_TIMER, TIM_IT_Update, ENABLE);
    TIM_Cmd(US_SENSOR_TIMER, ENABLE);

    return HAL_US_SENSOR_E_SUCCESS;
}


int hal_us_sensor_terminate(void)
{
    return HAL_US_SENSOR_E_SUCCESS;
}


int hal_us_sensor_open(void *params, HAL_US_SENSOR_HANDLE *handle)
{
    return HAL_US_SENSOR_E_SUCCESS;
}


int hal_us_sensor_close(HAL_US_SENSOR_HANDLE handle)
{
    return HAL_US_SENSOR_E_SUCCESS;
}


long hal_us_sensor_get_distance(HAL_US_SENSOR_HANDLE handle)
{

    return HAL_US_SENSOR_E_SUCCESS;
}


/**
 * @brief   Configures the Nested Vectored Interrupt Controller
 * @param   None
 * @retval  None
 */
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable SENSORS TIMER Interrupt handling */
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/**
 * @brief  Configures the different system clocks.
 * @param  None
 * @retval None
 */
void RCC_Configuration(void)
{
    /* GPIO clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    /* Timer clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
}


/**
 * @brief  Configure the GPIO pins.
 * @param  None
 * @retval None
 */
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

//    GPIO_InitStructure.GPIO_Pin = SENSOR_R10_PIN |
//                                  SENSOR_R5_PIN  |
//                                  SENSOR_F10_PIN |
//                                  SENSOR_F5_PIN  |
//                                  SENSOR_L10_PIN |
//                                  SENSOR_L5_PIN;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(SENSORS_PORT, &GPIO_InitStructure);
//
//    /* Enable Pins configuration */
//    GPIO_InitStructure.GPIO_Pin = SENSOR_EN_5_PIN |
//                                  SENSOR_EN_10_PIN;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(SENSORS_PORT, &GPIO_InitStructure);
//
//    GPIO_InitStructure.GPIO_Pin = SENSOR_CL_PIN |
//                                  SENSOR_CR_PIN;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/**********************
 * Interrupt handlers *
 **********************/

void TIM7_IRQHandler(void)
{
    static unsigned char etu = DEFAULT_ETU;

    if (TIM_GetFlagStatus(US_SENSOR_TIMER, TIM_FLAG_Update) == RESET)
    {
        return;
    }
}

#endif
