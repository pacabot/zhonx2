################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../pacabot/src/tests/oled/test_oled_exemple.c 

OBJS += \
./pacabot/src/tests/oled/test_oled_exemple.o 

C_DEPS += \
./pacabot/src/tests/oled/test_oled_exemple.d 


# Each subdirectory must supply rules for building sources it contributes
pacabot/src/tests/oled/%.o: ../pacabot/src/tests/oled/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D__FPU_USED -D__ASSEMBLY__ -DUSE_STDPERIPH_DRIVER -DSTM32F4xx -DSTM32F405RG -D__VFP_FP__ -I"/home/zhonx/workspace_eclipse_arm/zhonxII/cmsis" -I"/home/zhonx/workspace_eclipse_arm/zhonxII/cmsis_boot" -I"/home/zhonx/workspace_eclipse_arm/zhonxII/cmsis_lib/include" -I"/home/zhonx/workspace_eclipse_arm/zhonxII/pacabot/include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


