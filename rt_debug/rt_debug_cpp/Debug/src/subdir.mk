################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/rt_debug_main.cc \
../src/rt_debug_types.cc 

CC_DEPS += \
./src/rt_debug_main.d \
./src/rt_debug_types.d 

OBJS += \
./src/rt_debug_main.o \
./src/rt_debug_types.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cc src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/rt_debug_main.d ./src/rt_debug_main.o ./src/rt_debug_types.d ./src/rt_debug_types.o

.PHONY: clean-src

