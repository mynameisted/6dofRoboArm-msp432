################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-arm_18.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/Xing Long/workspace_v9_2/ArmController" --include_path="C:/Users/Xing Long/workspace_v9_2/ArmController/Debug" --include_path="C:/ti/simplelink_msp432p4_sdk_3_30_00_13/source" --include_path="C:/ti/simplelink_msp432p4_sdk_3_30_00_13/source/third_party/CMSIS/Include" --include_path="C:/ti/simplelink_msp432p4_sdk_3_30_00_13/source/ti/posix/ccs" --include_path="C:/ti/ccs920/ccs/tools/compiler/ti-cgt-arm_18.12.3.LTS/include" --advice:power=none -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/Xing Long/workspace_v9_2/ArmController/Debug/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-656588849:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-656588849-inproc

build-656588849-inproc: ../i2cmasterexample1.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs920/ccs/utils/sysconfig/sysconfig_cli.bat" -s "C:/ti/simplelink_msp432p4_sdk_3_30_00_13/.metadata/product.json" -o "syscfg" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_drivers_config.c: build-656588849 ../i2cmasterexample1.syscfg
syscfg/ti_drivers_config.h: build-656588849
syscfg/syscfg_c.rov.xs: build-656588849
syscfg/: build-656588849

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-arm_18.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/Xing Long/workspace_v9_2/ArmController" --include_path="C:/Users/Xing Long/workspace_v9_2/ArmController/Debug" --include_path="C:/ti/simplelink_msp432p4_sdk_3_30_00_13/source" --include_path="C:/ti/simplelink_msp432p4_sdk_3_30_00_13/source/third_party/CMSIS/Include" --include_path="C:/ti/simplelink_msp432p4_sdk_3_30_00_13/source/ti/posix/ccs" --include_path="C:/ti/ccs920/ccs/tools/compiler/ti-cgt-arm_18.12.3.LTS/include" --advice:power=none -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/Xing Long/workspace_v9_2/ArmController/Debug/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

msp432_mpu6050.obj: ../msp432_mpu6050.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-arm_18.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/Xing Long/workspace_v9_2/ArmController" --include_path="C:/Users/Xing Long/workspace_v9_2/ArmController/Debug" --include_path="C:/ti/simplelink_msp432p4_sdk_3_30_00_13/source" --include_path="C:/ti/simplelink_msp432p4_sdk_3_30_00_13/source/third_party/CMSIS/Include" --include_path="C:/ti/simplelink_msp432p4_sdk_3_30_00_13/source/ti/posix/ccs" --include_path="C:/ti/ccs920/ccs/tools/compiler/ti-cgt-arm_18.12.3.LTS/include" --include_path="C:/ti/ccs920/ccs/ccs_base/arm/include" --advice:power=none --define=__MSP432P401R__ -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/Xing Long/workspace_v9_2/ArmController/Debug/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


