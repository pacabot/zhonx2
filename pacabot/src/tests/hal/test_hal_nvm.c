/* test_hal_nvm.c */

#include "config/basetypes.h"
#include "config/errors.h"

#include "hal/hal_os.h"
#include "hal/hal_adc.h"
#include "hal/hal_beeper.h"
#include "hal/hal_led.h"
#include "hal/hal_nvm.h"
#include "hal/hal_pid.h"
#include "hal/hal_step_motor.h"
#include "hal/hal_sensor.h"
#include "hal/hal_serial.h"
#include "hal/hal_ui.h"

#include "app/app_def.h"

extern app_config app_context;

HAL_NVM_HANDLE nvm;

int test_hal_nvm(void)
{
    hal_nvm_write(app_context.nvm, (void *)0x08040000, "\xDE\xAD", 2);
    return 0;
}
