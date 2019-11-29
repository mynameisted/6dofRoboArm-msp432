################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../MSP_EXP432P401R_TIRTOS.cmd 

SYSCFG_SRCS += \
../i2cmasterexample1.syscfg 

C_SRCS += \
../LED.c \
./syscfg/ti_drivers_config.c \
../main_tirtos.c \
../msp432_bluetooth.c \
../msp432_mpu6050.c \
../msp432_sensors.c 

GEN_FILES += \
./syscfg/ti_drivers_config.c 

GEN_MISC_DIRS += \
./syscfg/ 

C_DEPS += \
./LED.d \
./syscfg/ti_drivers_config.d \
./main_tirtos.d \
./msp432_bluetooth.d \
./msp432_mpu6050.d \
./msp432_sensors.d 

OBJS += \
./LED.obj \
./syscfg/ti_drivers_config.obj \
./main_tirtos.obj \
./msp432_bluetooth.obj \
./msp432_mpu6050.obj \
./msp432_sensors.obj 

GEN_MISC_FILES += \
./syscfg/ti_drivers_config.h \
./syscfg/syscfg_c.rov.xs 

GEN_MISC_DIRS__QUOTED += \
"syscfg\" 

OBJS__QUOTED += \
"LED.obj" \
"syscfg\ti_drivers_config.obj" \
"main_tirtos.obj" \
"msp432_bluetooth.obj" \
"msp432_mpu6050.obj" \
"msp432_sensors.obj" 

GEN_MISC_FILES__QUOTED += \
"syscfg\ti_drivers_config.h" \
"syscfg\syscfg_c.rov.xs" 

C_DEPS__QUOTED += \
"LED.d" \
"syscfg\ti_drivers_config.d" \
"main_tirtos.d" \
"msp432_bluetooth.d" \
"msp432_mpu6050.d" \
"msp432_sensors.d" 

GEN_FILES__QUOTED += \
"syscfg\ti_drivers_config.c" 

C_SRCS__QUOTED += \
"../LED.c" \
"./syscfg/ti_drivers_config.c" \
"../main_tirtos.c" \
"../msp432_bluetooth.c" \
"../msp432_sensors.c" 

SYSCFG_SRCS__QUOTED += \
"../i2cmasterexample1.syscfg" 


