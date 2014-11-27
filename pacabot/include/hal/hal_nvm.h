/*---------------------------------------------------------------------------
 *
 *      hal_nvm.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __HAL_NVM_H__
#define __HAL_NVM_H__

/* Module Identifier */
#define HAL_NVM_MODULE_ID   6

/**
 * @defgroup HAL_NVM_Error_Codes Error Codes
 *
 * @ingroup HAL_NVM
 * @ingroup Error_Codes
 *
 * This chapter contains descriptions of all error codes used by the \link
 * HAL_NVM of the Hardware Abstraction Layer.
 *
 * @{
 */

/** \showinitializer No error */
#define HAL_NVM_E_SUCCESS  0

/** \showinitializer Generic error */
#define HAL_NVM_E_ERROR   MAKE_ERROR(HAL_NVM_MODULE_ID,1)




/** @}*/

/**
 *
 * @defgroup HAL_NVM HAL Non-Volatile Memory
 * This chapter contains descriptions of the interfaces of the functions
 * accessing the Non-Volatile Memory interface.
 *
 * @addtogroup HAL_NVM
 *
 * @{
 */

/** Opaque data type definition for the Non-Volatile Memory interface */
typedef void *HAL_NVM_HANDLE;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the Non-Volatile Memory interfaces.
 *
 * This function is Initializes the Non-Volatile Memory interface.
 *
 * @return #HAL_NVM_E_SUCCESS if the operation is successful,
 *         #HAL_NVM_E_ERROR otherwise.
 */
int hal_nvm_init(void);


/**
 * @brief Shutdowns the Non-Volatile Memory interfaces.
 *
 * It must perform all operations required to cleanup after the interface use.
 *
 * @return #HAL_NVM_E_SUCCESS if the operation is successful,
 *         #HAL_NVM_E_ERROR otherwise.
 */
int hal_nvm_terminate(void);


/**
* @brief Opens the Non-Volatile Memory interfaces.
*
* @return #HAL_NVM_E_SUCCESS if the operation is successful,
*         #HAL_NVM_E_ERROR otherwise.
*/
int hal_nvm_open(HAL_NVM_HANDLE *handle, void *params);


/**
* @brief Closes the Non-Volatile Memory interfaces.
*
* @return #HAL_NVM_E_SUCCESS if the operation is successful,
*         #HAL_NVM_E_ERROR otherwise.
*/
int hal_nvm_close(HAL_NVM_HANDLE handle);


/**
 * @brief Write in Non-Volatile Memory interface.
 *
 * This function writes in Non-Volatile Memory .
 *
 * @param[in]   handle   handle of the interface to close.
 *
 * @return #HAL_NVM_E_SUCCESS if the operation is successful,
 *		   #HAL_NVM_E_INVAL if the argument is invalid
 *         #HAL_NVM_E_ERROR otherwise.
 */
int hal_nvm_write(HAL_NVM_HANDLE handle, void *dst,
                  void *src, int length);


int hal_nvm_init_sector(HAL_NVM_HANDLE handle, unsigned long address);

#endif

