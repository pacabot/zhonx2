/*---------------------------------------------------------------------------
 *
 *      hal_lm629.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __HAL_LM629_H__
#define __HAL_LM629_H__

/* Module Identifier */
#define HAL_LM629_MODULE_ID   4

/************************************************************************                                                                           
 * Error codes                                                          *
 ************************************************************************/

/* Error codes */
#define HAL_LM629_E_SUCCESS    MAKE_ERROR(HAL_LM629_MODULE_ID, 0)
#define HAL_LM629_E_ERROR      MAKE_ERROR(HAL_LM629_MODULE_ID, 1)
#define HAL_LM629_E_BAD_HANDLE MAKE_ERROR(HAL_LM629_MODULE_ID, 2)
                                                                          
                                                                           
/************************************************************************
 * LM629 Constants                                                      *
 ************************************************************************/
#define LM629_CMD         0x00    // Command
#define LM629_DATA        0x01    // Data


/************************************************************************
 * LM629 Commands                                                       *
 ************************************************************************/
#define COMMAND_DFH       0x02    // Define Home                                                 
#define COMMAND_LFIL      0x1E    // Load FILter Parameters                                                 
#define COMMAND_LPEI      0x1B    // Load Position Error for Interrupt                                                 
#define COMMAND_LPES      0x1A    // Load Position Error for Stopping                                                 
#define COMMAND_LTRJ      0x1F    // Load Trajectory Parameters                                                 
#define COMMAND_MSKI      0x1C    // Mask Interrupts                                                 
#define COMMAND_RDDP      0x08    // Read Desired Position                                                 
#define COMMAND_RDDV      0x07    // ReaD Desired Velocity                                                 
#define COMMAND_RDIP      0x09    // ReaD Index Register                                                 
#define COMMAND_RDSUM     0x0D    // ReaD Integration Sum
#define COMMAND_RDRP      0x0A    // ReaD Real Position
#define COMMAND_RDRV      0x0B    // ReaD Real Velocity                          
#define COMMAND_RDSIGS    0x0C    // ReaD Signals Register                       
#define COMMAND_RESET     0x00    // RESET                                       
#define COMMAND_RSTI      0x1D    // Reset Interrupts                            
#define COMMAND_SBPA      0x20    // Set Breakpoint Absolute                     
#define COMMAND_SBPR      0x21    // Set Breakpoint Relative                     
#define COMMAND_SIP       0x03    // Set Index Position                          
#define COMMAND_STT       0x01    // Start Motion                                
#define COMMAND_UDF       0x04    // Update Filter                               


/************************************************************************
 * Status register values                                               *
 ************************************************************************/
#define STATUS_BUSY       0x01      // A 1 si le LM629 est occupé
#define STATUS_CMDERR     0x02      // Erreur de commande (lecture à la place d'écriture et inversement)
#define STATUS_TRJ_END    0x04      // Trajectoire terminée
#define STATUS_IDX_POS    0x08      // A 1 si on a modifié l'index de position
#define STATUS_DEP_ESP    0x10      // Dépassement d'espace pour la position
#define STATUS_POS_ERR    0x20      // Erreur de position excessive (par rapport aux limites fixées)
#define STATUS_PT_ARR     0x40      // Point d'arret atteint
#define STATUS_MOTOR_OFF  0x80      // Moteur éteint
#define STATUS_UNKNOWN    0xFF      // Status inconnu
                          

typedef void *HAL_LM629_HANDLE;

int hal_lm629_init(void);
int hal_lm629_terminate(void);
int hal_lm629_open(void *handle, void *params);
int hal_lm629_close(void *handle);
int hal_lm629_send_byte(unsigned char type, unsigned char value);
unsigned char hal_lm629_receive_byte(unsigned char type);
void hal_lm629_set_write_mode(unsigned char type);
void hal_lm629_set_read_mode(void);


#endif // __HAL_LM629_H__
