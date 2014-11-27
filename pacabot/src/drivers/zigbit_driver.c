/*---------------------------------------------------------------------------
 *
 *      zigbit_driver.c
 *
 *---------------------------------------------------------------------------*/

/* General declarations */
#include "config/basetypes.h"
#include "config/config.h"
#include "config/errors.h"

/* HAL declarations */
#include "hal/hal_zigbit.h"

/* Declarations for this module */
#include "drivers/zigbit_driver.h"

#include <string.h>

/******************
 * AT Command set *
 ******************/

/* Network management */
#define CMD_PAN_ID          "+WPANID"       // PAN ID
#define CMD_ACTIVE_CHANNEL  "+WCHAN"        // Active channel
#define CMD_CHANNEL_MASK    "+WCHMASK"      // Channel mask
#define CMD_CHANNEL_PAGE    "+WCHPAGE"      // Channel page
#define CMD_LEAVE_NETWORK   "+WLEAVE"       // Leave the network
#define CMD_JOIN_NETWORK    "+WJOIN"        // Start/Join to network
#define CMD_NTWK_STATUS     "+WNWK"         // Request for networking status
#define CMD_PARENT_ADDR     "+WPARENT"      // Request for parent address
#define CMD_CHILDREN_ADDR   "+WCHILDREN"    // Request for children addresses
#define CMD_AUTO_NETWORK    "+WAUTONET"     // Automatic networking

/* General Mode Management */
#define CMD_PWR_MANAGEMENT  "+WPWR"         // Power management
#define CMD_SLEEP           "+WSLEEP"       // Force to sleep
#define CMD_SET_NODE_ROLE   "+WROLE"        // Set node role
#define CMD_SET_SYNC_PERIOD "+WSYNCPRD"     // Set period for tracking the end devices
#define CMD_TX_PWR_LVL      "+WTXPWR"       // TX power level
#define CMD_REQ_LQI         "+WLQI"         // Request for LQI
#define CMD_REQ_RSSI        "+WRSSI"        // Request for RSSI
#define CMD_SET_ADDR_MODE   "S30"           // Set network addressing mode
#define CMD_PWM1_CFG        "S140"          // PWM1 configuration
#define CMD_PWM2_CFG        "S141"          // PWM2 configuration
#define CMD_PWM3_CFG        "S142"          // PWM3 configuration
#define CMD_PWM1_FRQ        "S143"          // PWM1 Frequency Control
#define CMD_PWM2_FRQ        "S144"          // PWM2 Frequency Control
#define CMD_PWM3_FRQ        "S145"          // PWM3 Frequency Control
#define CMD_PWM1_DUTY       "S146"          // PWM1 duty cycle control
#define CMD_PWM2_DUTY       "S147"          // PWM2 duty cycle control
#define CMD_PWM3_DUTY       "S148"          // PWM3 duty cycle control

/* Data transmission */
#define CMD_SEND_DATA       "D"             // Send data to specific node
#define CMD_SEND_BROADCAST  "DU"            // Send broadcast data
#define CMD_SEND_S_REGISTER "DS"            // Send S-register value to specific node
#define CMD_PING            "+WPING"        // Ping the node

/* Generic control */
#define CMD_HELP            "&H"            // Help
#define CMD_PRINT_PARAMS    "%H"            // Display parameters and S-register values
#define CMD_PRINT_PROD_INFO "I"             // Display product identification information
#define CMD_WARM_RESET      "Z"             // Warm Reset
#define CMD_SET_FACTORY     "&F"            // Set to factory-defined configuration

/* Host interface commands */
#define CMD_TERMINAT_CHAR   "S3"            // Termination character
#define CMD_RESP_CHAR       "S4"            // Response formatting character
#define CMD_EDITING_CHAR    "S5"            // Command editing character
#define CMD_ECHO            "E"             // Command echo
#define CMD_DELETE_RES_CODE "Q"             // Result code suppression
#define CMD_RESP_FORMAT     "V"             // Response format
#define CMD_RES_CODE_SELECT "X"             // Result code selection
#define CMD_UART_COM_RATE   "+IPR"          // Serial port communication rate
#define CMD_UART_FLOW_CTRL  "+IFC"          // Serial port flow control
#define CMD_DTR_BEHAVIOR    "&D"            // DTR behavior
#define CMD_LAST_RES_CODE   "S0"            // Request for the latest result code

#define CMD_RESPONSE_SUCCESS    "OK"        // The command has been executed correctly

/* Timeout for receiving a command response from the Zigbit module */
#define CMD_RESPONSE_TIMEOUT 2000

/* Macros */
#define BUILD_AT_COMMAND(_cmd_)  ("AT"_cmd_)

typedef struct {
    HAL_ZIGBIT_HANDLE h;
} zigbit_module;

static zigbit_module zigbit;


int zigbit_driver_init(void)
{
    int             rv;
    unsigned char   resp[10];
    unsigned int    resp_length;

    rv = hal_zigbit_init();
    if (rv != HAL_ZIGBIT_E_SUCCESS)
    {
        return rv;
    }

    rv = hal_zigbit_open(NULL, &(zigbit.h));
    if (rv != HAL_ZIGBIT_E_SUCCESS)
    {
        return rv;
    }

    // Send Initialization AT command
    rv = hal_zigbit_send(zigbit.h, BUILD_AT_COMMAND(CMD_RES_CODE_SELECT), 2 + sizeof(CMD_RES_CODE_SELECT));
    if (rv != HAL_ZIGBIT_E_SUCCESS)
    {
        return rv;
    }

    // Get back the response code from the Zigbit module
    rv = hal_zigbit_receive(zigbit.h, resp, &resp_length, CMD_RESPONSE_TIMEOUT);
    if (rv != HAL_ZIGBIT_E_SUCCESS)
    {
        return rv;
    }

    /* Analyze the Zigbit module response */
    if (strcmp((const char *)resp, CMD_RESPONSE_SUCCESS) != 0)
    {
        return HAL_ZIGBIT_E_ERROR;
    }

    return ZIGBIT_DRIVER_E_SUCCESS;
}
