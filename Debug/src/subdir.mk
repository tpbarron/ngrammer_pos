################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Ngram.cpp \
../src/Pos.cpp \
../src/Template.cpp \
../src/Tokenizer.cpp \
../src/ngrammer.cpp 

OBJS += \
./src/Ngram.o \
./src/Pos.o \
./src/Template.o \
./src/Tokenizer.o \
./src/ngrammer.o 

CPP_DEPS += \
./src/Ngram.d \
./src/Pos.d \
./src/Template.d \
./src/Tokenizer.d \
./src/ngrammer.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/python2.7/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ngrammer.o: ../src/ngrammer.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/ngrammer.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


