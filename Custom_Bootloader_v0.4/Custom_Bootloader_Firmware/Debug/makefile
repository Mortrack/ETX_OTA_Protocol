################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Custom_Bootloader_Firmware
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Custom_Bootloader_Firmware.elf \

MAP_FILES += \
Custom_Bootloader_Firmware.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Custom_Bootloader_Firmware.list \

OBJCOPY_HEX += \
Custom_Bootloader_Firmware.hex \

OBJCOPY_BIN += \
Custom_Bootloader_Firmware.bin \


# All Target
all: main-build

# Main-build Target
main-build: Custom_Bootloader_Firmware.elf secondary-outputs

# Tool invocations
Custom_Bootloader_Firmware.elf Custom_Bootloader_Firmware.map: $(OBJS) $(USER_OBJS) C:\Users\cmira\OneDrive\Escritorio\GitHub\ETX_OTA_Protocol\Custom_Bootloader_v0.4\Custom_Bootloader_Firmware\STM32F103C8TX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "Custom_Bootloader_Firmware.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"C:\Users\cmira\OneDrive\Escritorio\GitHub\ETX_OTA_Protocol\Custom_Bootloader_v0.4\Custom_Bootloader_Firmware\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Custom_Bootloader_Firmware.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Custom_Bootloader_Firmware.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Custom_Bootloader_Firmware.list"
	@echo 'Finished building: $@'
	@echo ' '

Custom_Bootloader_Firmware.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "Custom_Bootloader_Firmware.hex"
	@echo 'Finished building: $@'
	@echo ' '

Custom_Bootloader_Firmware.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Custom_Bootloader_Firmware.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Custom_Bootloader_Firmware.bin Custom_Bootloader_Firmware.elf Custom_Bootloader_Firmware.hex Custom_Bootloader_Firmware.list Custom_Bootloader_Firmware.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
