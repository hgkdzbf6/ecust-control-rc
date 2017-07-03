################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../libs/ViconDataStreamSDK_CPPTest.cpp 

OBJS += \
./libs/ViconDataStreamSDK_CPPTest.o 

CPP_DEPS += \
./libs/ViconDataStreamSDK_CPPTest.d 


# Each subdirectory must supply rules for building sources it contributes
libs/%.o: ../libs/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


