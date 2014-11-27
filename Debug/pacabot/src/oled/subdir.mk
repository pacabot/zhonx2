################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../pacabot/src/oled/smallfonts.c \
../pacabot/src/oled/ssd1306.c \
../pacabot/src/oled/ssd1306_peripherals.c 

OBJS += \
./pacabot/src/oled/smallfonts.o \
./pacabot/src/oled/ssd1306.o \
./pacabot/src/oled/ssd1306_peripherals.o 

C_DEPS += \
./pacabot/src/oled/smallfonts.d \
./pacabot/src/oled/ssd1306.d \
./pacabot/src/oled/ssd1306_peripherals.d 


# Each subdirectory must supply rules for building sources it contributes
pacabot/src/oled/%.o: ../pacabot/src/oled/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D__FPU_USED -D__ASSEMBLY__ -DUSE_STDPERIPH_DRIVER -DSTM32F4xx -DSTM32F405RG -D__VFP_FP__ -I"/home/colin/workspace_eclipse_arm/ZHONX_II_COLIN/cmsis" -I"/home/colin/workspace_eclipse_arm/ZHONX_II_COLIN/cmsis_boot" -I"/home/colin/workspace_eclipse_arm/ZHONX_II_COLIN/cmsis_lib/include" -I"/home/colin/workspace_eclipse_arm/ZHONX_II_COLIN/pacabot/include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


