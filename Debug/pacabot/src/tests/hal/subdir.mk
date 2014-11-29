################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../pacabot/src/tests/hal/test_hal_adc.c \
../pacabot/src/tests/hal/test_hal_beeper.c \
../pacabot/src/tests/hal/test_hal_color_sensor.c \
../pacabot/src/tests/hal/test_hal_led.c \
../pacabot/src/tests/hal/test_hal_nvm.c \
../pacabot/src/tests/hal/test_hal_sensor.c \
../pacabot/src/tests/hal/test_hal_serial.c \
../pacabot/src/tests/hal/test_hal_step_motor.c \
../pacabot/src/tests/hal/test_hal_ui.c 

OBJS += \
./pacabot/src/tests/hal/test_hal_adc.o \
./pacabot/src/tests/hal/test_hal_beeper.o \
./pacabot/src/tests/hal/test_hal_color_sensor.o \
./pacabot/src/tests/hal/test_hal_led.o \
./pacabot/src/tests/hal/test_hal_nvm.o \
./pacabot/src/tests/hal/test_hal_sensor.o \
./pacabot/src/tests/hal/test_hal_serial.o \
./pacabot/src/tests/hal/test_hal_step_motor.o \
./pacabot/src/tests/hal/test_hal_ui.o 

C_DEPS += \
./pacabot/src/tests/hal/test_hal_adc.d \
./pacabot/src/tests/hal/test_hal_beeper.d \
./pacabot/src/tests/hal/test_hal_color_sensor.d \
./pacabot/src/tests/hal/test_hal_led.d \
./pacabot/src/tests/hal/test_hal_nvm.d \
./pacabot/src/tests/hal/test_hal_sensor.d \
./pacabot/src/tests/hal/test_hal_serial.d \
./pacabot/src/tests/hal/test_hal_step_motor.d \
./pacabot/src/tests/hal/test_hal_ui.d 


# Each subdirectory must supply rules for building sources it contributes
pacabot/src/tests/hal/%.o: ../pacabot/src/tests/hal/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D__FPU_USED -D__ASSEMBLY__ -DUSE_STDPERIPH_DRIVER -DSTM32F4xx -DSTM32F405RG -D__VFP_FP__ -I"/home/colin/workspace_eclipse_arm/zhonxII/cmsis" -I"/home/colin/workspace_eclipse_arm/zhonxII/cmsis_boot" -I"/home/colin/workspace_eclipse_arm/zhonxII/cmsis_lib/include" -I"/home/colin/workspace_eclipse_arm/zhonxII/pacabot/include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


