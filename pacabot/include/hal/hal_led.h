/*---------------------------------------------------------------------------
 *
 *      hal_led.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __HAL_LED_H__
#define __HAL_LED_H__

/* Module Identifier */
#define HAL_LED_MODULE_ID   3

/**
 * @defgroup HAL_LED_Error_Codes Led Error Codes
 *
 * @ingroup HAL_LED
 * @ingroup Error_Codes
 *
 * This chapter contains descriptions of all error codes used by the \link
 * HAL_LED Led Interface \endlink of the Hardware
 * Abstraction Layer.
 *
 * @{
 */

/** \showinitializer No error */
#define HAL_LED_E_SUCCESS  0
/** \showinitializer Generic error */
#define HAL_LED_E_ERROR   MAKE_ERROR(HAL_LED_MODULE_ID, 0x1)

/** @}*/

/**
 *
 * @defgroup HAL_LED HAL Led
 * This chapter contains descriptions of the interfaces of the functions
 * accessing the Led interface.
 *
 * @addtogroup HAL_LED
 *
 * @{
 */


/*------------------------------------------------------------------------------
   LED States
------------------------------------------------------------------------------*/
#define HAL_LED_STATE_OFF 0
#define HAL_LED_STATE_ON  1


/*------------------------------------------------------------------------------
   LED Colors
------------------------------------------------------------------------------*/

/** Color code for the default color */
#define HAL_LED_COLOR_DEFAULT 0

/** Color code for the red LED */
#define HAL_LED_COLOR_RED     2

/** Color code for the green LED */
#define HAL_LED_COLOR_GREEN   3

/** Color code for the orange LED */
#define HAL_LED_COLOR_ORANGE  4

/** Color code for the blue LED */
#define HAL_LED_COLOR_BLUE    1


/** Opaque data type definition for the Led interface */
typedef void *HAL_LED_HANDLE;

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Initializes the Led interface.
 *
 * This function initializes the Led interface.
 * This function is called once when the system starts up, before
 * any other call to the HAL for this interface. It must perform all actions
 * required to prepare the service for use. This includes the allocation
 * and/or initialization of all structures and variables, and the loading of
 * any required drivers.
 *
 * @return #HAL_LED_E_SUCCESS if the operation is successful,
 *         #HAL_LED_E_ERROR otherwise.
 */
int hal_led_init(void);


/**
 * @brief Shutdowns the Led interface.
 *
 * This function is called once when the system shuts down.
 * It must perform all operations required to cleanup after the interface use.
 * This includes the freeing of any allocated memory, the reinitialization of
 * any static memory (if needed), and the unloading of any drivers.
 *
 * @return #HAL_LED_E_SUCCESS if the operation is successful,
 *         #HAL_LED_E_ERROR otherwise.
 */
int hal_led_terminate(void);


/**
 * @brief Opens a specific Led interface.
 *
 * This function opens a specific Led interface for use by the
 * application.
 *
 * @param[in]  param    parameter identifying the led to open.
 * @param[out] handle	valid pointer to a handle of the led to open.
 *
 * @return #HAL_LED_E_SUCCESS if the operation is successful,
 *         #HAL_LED_E_ERROR otherwise.
 */
int hal_led_open(HAL_LED_HANDLE *handle, void *param);


/**
 * @brief Closes a specific Led device.
 *
 * This function closes a specific Led device and releases
 * its resources.
 *
 * @param[in]   handle   handle of the led to close.
 *
 * @return #HAL_LED_E_SUCCESS if the operation is successful,
 *         #HAL_LED_E_ERROR otherwise.
 */
int hal_led_close(HAL_LED_HANDLE handle);


/**
 * @brief Turns a LED on.
 *
 * This function turns the LED on for a specified duration.
 *
 * @param[in] handle	handle of the Led interface.
 * @param[in] color     color of the LED. The possible values are the
 *                      following:
 *                         \li #HAL_LED_COLOR_DEFAULT for a default color
 *                         \li #HAL_LED_COLOR_RED for a red LED
 *                         \li #HAL_LED_COLOR_ORANGE for an orange LED
 *                         \li #HAL_LED_COLOR_GREEN for a green LED
 *                         \li #HAL_LED_COLOR_BLUE for a blue LED
 * @param[in] duration	duration in milliseconds of the LED.
 *                      There are two special cases for duration, as follows:
 *                          \li If duration == FOREVER, the function will wait
 *                              until something is available on the associated
 *                              interface's input.
 *                          \li If duration == IMMEDIATE, the function will
 *                              immediately read the input.
 *
 * @return #HAL_LED_E_SUCCESS if the operation is successful,
 *         #HAL_LED_E_ERROR otherwise.
 */
int hal_led_toggle(HAL_LED_HANDLE handle,
                    unsigned char color,
                    unsigned long duration);


/**
 * @brief Reset the leds status.
 *
 * This function switches all the leds off.
 *
 * @param[in] handle	handle of the Led interface.
 *
 * @return #HAL_LED_E_SUCCESS if the operation is successful,
 *         #HAL_LED_E_ERROR otherwise.
 */
int hal_led_reset(HAL_LED_HANDLE handle);


/**
 * @brief Set a led state.
 *
 * This function switches the state of a led.
 *
 * @param[in] handle	handle of the Led interface.
 * @param[in] color     color of the LED. The possible values are the
 *                      following:
 *                         \li #HAL_LED_COLOR_DEFAULT for a default color
 *                         \li #HAL_LED_COLOR_RED for a red LED
 *                         \li #HAL_LED_COLOR_ORANGE for an orange LED
 *                         \li #HAL_LED_COLOR_GREEN for a green LED
 *                         \li #HAL_LED_COLOR_BLUE for a blue LED
 * @param[in] state     led state. The possible values are as following:
 *                         \li #HAL_LED_STATE_ON to switch on the led
 *                         \li #HAL_LED_STATE_OFF to switch off the led
 *
 * @return #HAL_LED_E_SUCCESS if the operation is successful,
 *         #HAL_LED_E_ERROR otherwise.
 */
int hal_led_set_state(HAL_LED_HANDLE handle,
                      unsigned char color, unsigned char state);

#ifdef __cplusplus
}
#endif

 /** @} */

#endif /* __HAL_LED_H__ */

