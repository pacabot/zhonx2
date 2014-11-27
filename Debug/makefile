################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include syscalls/subdir.mk
-include pacabot/src/util/subdir.mk
-include pacabot/src/tests/solvMaze/subdir.mk
-include pacabot/src/tests/remote_control/subdir.mk
-include pacabot/src/tests/oled/subdir.mk
-include pacabot/src/tests/hal/subdir.mk
-include pacabot/src/tests/drivers/subdir.mk
-include pacabot/src/tests/calibration/subdir.mk
-include pacabot/src/oled/subdir.mk
-include pacabot/src/hal/hal_zigbit/subdir.mk
-include pacabot/src/hal/hal_us_sensor/subdir.mk
-include pacabot/src/hal/hal_ui/subdir.mk
-include pacabot/src/hal/hal_step_motor/subdir.mk
-include pacabot/src/hal/hal_serial/subdir.mk
-include pacabot/src/hal/hal_sensor/subdir.mk
-include pacabot/src/hal/hal_pid/subdir.mk
-include pacabot/src/hal/hal_os/subdir.mk
-include pacabot/src/hal/hal_nvm/subdir.mk
-include pacabot/src/hal/hal_lm629/subdir.mk
-include pacabot/src/hal/hal_led/subdir.mk
-include pacabot/src/hal/hal_beeper/subdir.mk
-include pacabot/src/hal/hal_adc/subdir.mk
-include pacabot/src/drivers/subdir.mk
-include pacabot/src/app/subdir.mk
-include cmsis_lib/source/subdir.mk
-include cmsis_boot/startup/subdir.mk
-include cmsis_boot/subdir.mk
-include ZHONX_II_M4/Debug/obj/subdir.mk
-include ZHONX_II_M4/Debug/bin/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
ZHONX_II_COLIN.hex \

SECONDARY_SIZE += \
ZHONX_II_COLIN.siz \


# All Target
all: ZHONX_II_COLIN.elf secondary-outputs

# Tool invocations
ZHONX_II_COLIN.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -T "/home/colin/workspace_eclipse_arm/ZHONX_II_COLIN/STM32F405RG_FLASH.ld" -Xlinker --gc-sections -Wl,-Map,"ZHONX_II_COLIN.map" -Wl,-cref,-u,Reset_Handler -o "ZHONX_II_COLIN.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

ZHONX_II_COLIN.hex: ZHONX_II_COLIN.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "ZHONX_II_COLIN.elf"  "ZHONX_II_COLIN.hex"
	@echo 'Finished building: $@'
	@echo ' '

ZHONX_II_COLIN.siz: ZHONX_II_COLIN.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "ZHONX_II_COLIN.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(SECONDARY_FLASH)$(S_DEPS)$(S_UPPER_DEPS) ZHONX_II_COLIN.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
