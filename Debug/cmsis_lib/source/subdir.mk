################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cmsis_lib/source/misc.c \
../cmsis_lib/source/stm32f4xx_adc.c \
../cmsis_lib/source/stm32f4xx_crc.c \
../cmsis_lib/source/stm32f4xx_dma.c \
../cmsis_lib/source/stm32f4xx_exti.c \
../cmsis_lib/source/stm32f4xx_flash.c \
../cmsis_lib/source/stm32f4xx_gpio.c \
../cmsis_lib/source/stm32f4xx_pwr.c \
../cmsis_lib/source/stm32f4xx_rcc.c \
../cmsis_lib/source/stm32f4xx_sdio.c \
../cmsis_lib/source/stm32f4xx_spi.c \
../cmsis_lib/source/stm32f4xx_syscfg.c \
../cmsis_lib/source/stm32f4xx_tim.c \
../cmsis_lib/source/stm32f4xx_usart.c 

OBJS += \
./cmsis_lib/source/misc.o \
./cmsis_lib/source/stm32f4xx_adc.o \
./cmsis_lib/source/stm32f4xx_crc.o \
./cmsis_lib/source/stm32f4xx_dma.o \
./cmsis_lib/source/stm32f4xx_exti.o \
./cmsis_lib/source/stm32f4xx_flash.o \
./cmsis_lib/source/stm32f4xx_gpio.o \
./cmsis_lib/source/stm32f4xx_pwr.o \
./cmsis_lib/source/stm32f4xx_rcc.o \
./cmsis_lib/source/stm32f4xx_sdio.o \
./cmsis_lib/source/stm32f4xx_spi.o \
./cmsis_lib/source/stm32f4xx_syscfg.o \
./cmsis_lib/source/stm32f4xx_tim.o \
./cmsis_lib/source/stm32f4xx_usart.o 

C_DEPS += \
./cmsis_lib/source/misc.d \
./cmsis_lib/source/stm32f4xx_adc.d \
./cmsis_lib/source/stm32f4xx_crc.d \
./cmsis_lib/source/stm32f4xx_dma.d \
./cmsis_lib/source/stm32f4xx_exti.d \
./cmsis_lib/source/stm32f4xx_flash.d \
./cmsis_lib/source/stm32f4xx_gpio.d \
./cmsis_lib/source/stm32f4xx_pwr.d \
./cmsis_lib/source/stm32f4xx_rcc.d \
./cmsis_lib/source/stm32f4xx_sdio.d \
./cmsis_lib/source/stm32f4xx_spi.d \
./cmsis_lib/source/stm32f4xx_syscfg.d \
./cmsis_lib/source/stm32f4xx_tim.d \
./cmsis_lib/source/stm32f4xx_usart.d 


# Each subdirectory must supply rules for building sources it contributes
cmsis_lib/source/%.o: ../cmsis_lib/source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D__FPU_USED -D__ASSEMBLY__ -DUSE_STDPERIPH_DRIVER -DSTM32F4xx -DSTM32F405RG -D__VFP_FP__ -I"/home/colin/workspace_eclipse_arm/ZHONX_II_COLIN/cmsis" -I"/home/colin/workspace_eclipse_arm/ZHONX_II_COLIN/cmsis_boot" -I"/home/colin/workspace_eclipse_arm/ZHONX_II_COLIN/cmsis_lib/include" -I"/home/colin/workspace_eclipse_arm/ZHONX_II_COLIN/pacabot/include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


