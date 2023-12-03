################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Board/filesystem.c \
../Core/Src/Board/lis3mdl.c \
../Core/Src/Board/w25q80dv.c 

OBJS += \
./Core/Src/Board/filesystem.o \
./Core/Src/Board/lis3mdl.o \
./Core/Src/Board/w25q80dv.o 

C_DEPS += \
./Core/Src/Board/filesystem.d \
./Core/Src/Board/lis3mdl.d \
./Core/Src/Board/w25q80dv.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Board/%.o Core/Src/Board/%.su Core/Src/Board/%.cyclo: ../Core/Src/Board/%.c Core/Src/Board/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Board

clean-Core-2f-Src-2f-Board:
	-$(RM) ./Core/Src/Board/filesystem.cyclo ./Core/Src/Board/filesystem.d ./Core/Src/Board/filesystem.o ./Core/Src/Board/filesystem.su ./Core/Src/Board/lis3mdl.cyclo ./Core/Src/Board/lis3mdl.d ./Core/Src/Board/lis3mdl.o ./Core/Src/Board/lis3mdl.su ./Core/Src/Board/w25q80dv.cyclo ./Core/Src/Board/w25q80dv.d ./Core/Src/Board/w25q80dv.o ./Core/Src/Board/w25q80dv.su

.PHONY: clean-Core-2f-Src-2f-Board

