/*---------------------------------------------------------------------------
 *
 *      hal_nvm.c
 *
 *---------------------------------------------------------------------------*/

/* General declarations */
#include "config/basetypes.h"
#include "config/config.h"
#include "config/errors.h"

/* Declarations for this module */
#include "hal/hal_nvm.h"

#include <stdio.h>
#include <string.h>

/* ST Lib declarations */
#include "stm32f4xx_flash.h"


// NVM Zone definition
#define HAL_NVM_START_ADDR              ((uint32_t)0x08041000)
#define HAL_NVM_END_ADDR                ((uint32_t)0x0807FFFF)

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

static int get_sector(unsigned long address);

// Just to give something in the open() function
char hal_nvm_handle = 1;


/**
* @brief Initializes the Non-Volatile Memory interfaces.
*
* This function initializes the Non-Volatile Memory interface.
*
* @return #HAL_NVM_E_SUCCESS if the operation is successful,
*         #HAL_NVM_E_ERROR otherwise.
*/
int hal_nvm_init(void)
{
    return HAL_NVM_E_SUCCESS;
}


/**
* @brief Shutdowns the Non-Volatile Memory interfaces.
*
* It must perform all operations required to cleanup after the interface use.
*
* @return #HAL_NVM_E_SUCCESS if the operation is successful,
*         #HAL_NVM_E_ERROR otherwise.
*/
int hal_nvm_terminate(void)
{
    return HAL_NVM_E_SUCCESS;
}


/**
* @brief Opens the Non-Volatile Memory interfaces.
*
* @return #HAL_NVM_E_SUCCESS if the operation is successful,
*         #HAL_NVM_E_ERROR otherwise.
*/
int hal_nvm_open(HAL_NVM_HANDLE *handle, void *params)
{
    UNUSED(params);
    *handle = (HAL_NVM_HANDLE)&hal_nvm_handle;

    return HAL_NVM_E_SUCCESS;
}


/**
* @brief Closes the Non-Volatile Memory interfaces.
*
* @return #HAL_NVM_E_SUCCESS if the operation is successful,
*         #HAL_NVM_E_ERROR otherwise.
*/
int hal_nvm_close(HAL_NVM_HANDLE handle)
{
    return HAL_NVM_E_SUCCESS;
}


/**
* @brief Read from Non-Volatile Memory.
*
* The function reads from Non-Volatile Memory.
*
* @param[in] params    params for the connection.
* @param[out] handle    valid pointer toward a HAL_NVM_HANDLE handle
*
* @return #HAL_NVM_E_SUCCESS if the operation is successful,
*         #HAL_NVM_E_INVAL if one argument is invalid
*         #HAL_NVM_E_ERROR otherwise.
*/
int hal_nvm_read(HAL_NVM_HANDLE *handle,
                 unsigned char *dest, unsigned char *src, int length)
{
    // Check handle
    if (!handle)
    {
        return HAL_NVM_E_ERROR;
    }

    // Copy data
    memcpy(dest, src, length);

    return HAL_NVM_E_SUCCESS;
}


/**
* @brief Write in Non-Volatile Memory.
*
* The function writes in Non-Volatile Memory.
*
* @param[in] params    params for the connection.
* @param[out] handle    valid pointer toward a HAL_NVM_HANDLE handle
*
* @return #HAL_NVM_E_SUCCESS if the operation is successful,
*         #HAL_NVM_E_INVAL if one argument is invalid
*         #HAL_NVM_E_ERROR otherwise.
*/
int hal_nvm_write(HAL_NVM_HANDLE handle,
                  void *dst, void *src, int length)
{
    unsigned char *d = (unsigned char *)dst;
    unsigned char *s = (unsigned char *)src;

    /* Enable the flash control register access */
    FLASH_Unlock();

    do
    {
        FLASH_ProgramByte((unsigned int)d, *s);
        d++;
        s++;
    }
    while(length--);

    /* Lock the Flash to disable the flash control register access (recommended
       to protect the FLASH memory against possible unwanted operation) */
    FLASH_Lock();
    return HAL_NVM_E_SUCCESS;

//error:

//    return HAL_NVM_E_ERROR;
}


int hal_nvm_init_sector(HAL_NVM_HANDLE handle, unsigned long address)
{
    int sector = get_sector(address);
    int rv = HAL_NVM_E_SUCCESS;

    FLASH_Unlock();

    if (FLASH_EraseSector(sector, VoltageRange_3) != FLASH_COMPLETE)
    {
        rv = HAL_NVM_E_ERROR;
    }

    FLASH_Lock();

    return rv;
}



/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
static int get_sector(unsigned long address)
{
    int sector = 0;

    if ((address < ADDR_FLASH_SECTOR_1) && (address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_Sector_0;
    }
    else if ((address < ADDR_FLASH_SECTOR_2)
             && (address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_Sector_1;
    }
    else if ((address < ADDR_FLASH_SECTOR_3)
             && (address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_Sector_2;
    }
    else if ((address < ADDR_FLASH_SECTOR_4)
             && (address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_Sector_3;
    }
    else if ((address < ADDR_FLASH_SECTOR_5)
             && (address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_Sector_4;
    }
    else if ((address < ADDR_FLASH_SECTOR_6)
             && (address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_Sector_5;
    }
    else if ((address < ADDR_FLASH_SECTOR_7)
             && (address >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_Sector_6;
    }
    else if ((address < ADDR_FLASH_SECTOR_8)
             && (address >= ADDR_FLASH_SECTOR_7))
    {
        sector = FLASH_Sector_7;
    }
    else if ((address < ADDR_FLASH_SECTOR_9)
             && (address >= ADDR_FLASH_SECTOR_8))
    {
        sector = FLASH_Sector_8;
    }
    else if ((address < ADDR_FLASH_SECTOR_10)
             && (address >= ADDR_FLASH_SECTOR_9))
    {
        sector = FLASH_Sector_9;
    }
    else if ((address < ADDR_FLASH_SECTOR_11)
             && (address >= ADDR_FLASH_SECTOR_10))
    {
        sector = FLASH_Sector_10;
    }

    return sector;
}
