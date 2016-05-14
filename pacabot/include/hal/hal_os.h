/*---------------------------------------------------------------------------
 *
 *      hal_os.h
 *       
 *---------------------------------------------------------------------------*/

#ifndef __HAL_OS_H__
#define __HAL_OS_H__

/* Module Identifier */
#define HAL_OS_MODULE_ID    7

/**
 * @defgroup HAL_OS HAL OS
 * This chapter contains descriptions of the interfaces of the functions
 * accessing the underlying Operating System.
 *
 * @addtogroup HAL_OS
 *
 * @{
 */

#define HAL_OS_SUCCESS          0
#define HAL_OS_ERROR            1

#define printf hal_os_diag

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Allocates the permanent resources used by the HAL OS implementation.
 *
 * This function is called once when the system starts up, before
 * any other call to the HAL interfaces. It must perform all actions
 * required to prepare the OS service for use. This includes the allocation 
 * and/or initialization of all structures and variables, and the loading of 
 * any required drivers.
 *
 * @return #HAL_OS_SUCCESS if the operation is successful, 
 *         #HAL_OS_ERROR otherwise. 
 */
int hal_os_init(void);

/**
 * Releases the resources permanently allocated by the HAL OS implementation.
 *
 * This function is called once when the system shuts down. 
 * It must perform all operations required to cleanup after the interface use. 
 * This includes the freeing of any allocated memory, the reinitialization of 
 * any static memory (if needed), and the unloading of any drivers.
 *
 * @return #HAL_OS_SUCCESS if the operation is successful, 
 *         #HAL_OS_ERROR otherwise. 
 */
int hal_os_terminate(void);


/*
 * This function returns the current system ticks value.
 *
 */
unsigned long hal_os_get_systicks(void);


/*
 * This function induce a delay as specified in parameter.
 *
 * [in] delay in milliseconds.
 *
 * @return #HAL_OS_SUCCESS if the operation is successful
 *
 */
int HAL_Delay(unsigned long ms);

/**
 * @brief Prints a string on the trace output.
 *
 * The function prints a string on the trace output.
 *
 * @param   fmt 		Format control.
 *
 * @param   parameters  Optional arguments.
 *
 * @return #HAL_OS_SUCCESS if the operation is successful, 
 *         #HAL_OS_ERROR otherwise. 
 */
int hal_os_diag(const char *fmt, ...);

int hal_os_log(const char *fmt, ...);

void sigHandler(int signum);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __HAL_OS_H__ */
