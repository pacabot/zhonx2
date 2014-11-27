#include "config/config.h"
#include "config/basetypes.h"
#include "config/errors.h"

#include "hal/hal_lm629.h"


int hal_lm629_init(void)
{
    return HAL_LM629_E_SUCCESS;
}


int hal_lm629_terminate(void)
{
    return HAL_LM629_E_SUCCESS;
}


int hal_lm629_open(void *handle, void *params)
{
    UNUSED(handle);
    UNUSED(params);

    return HAL_LM629_E_SUCCESS;
}


int hal_lm629_close(void *handle)
{
    UNUSED(handle);

    return HAL_LM629_E_SUCCESS;
}


/**
 * @brief Sends a byte to LM629
 * 
 * @param type the type of data to be sent to LM629
 *        Can take two values:
 *          - LM629_CMD
 *          - LM629_DATA
 *
 * @param value is the COMMAND or the DATA to be sent
 */
int hal_lm629_send_byte(unsigned char type, unsigned char value)
{
    UNUSED(type);
    UNUSED(value);

    return HAL_LM629_E_SUCCESS;
}


/**
 * @brief Receives a byte from LM629
 *
 * @param type of the data to receive
 */
unsigned char hal_lm629_receive_byte(unsigned char type)
{
    UNUSED(type);

    return 0x00;
}


/**
 * @brief Sets the pins connected to the LM629 as output
 */
void hal_lm629_set_write_mode(unsigned char type)
{
    UNUSED(type);
}


/**
 * @brief Sets the pins connected to the LM629 as input
 */
void hal_lm629_set_read_mode(void)
{
}
