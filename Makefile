TARGET := DELTAH743

BUILD_DIR := build/$(TARGET)

CROSS_COMPILE ?= arm-none-eabi-

CC      := $(CROSS_COMPILE)gcc
AS      := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE    := $(CROSS_COMPILE)size

ARCH_FLAGS := \
	-mcpu=cortex-m7 \
	-mthumb \
	-mfpu=fpv5-sp-d16 \
	-mfloat-abi=hard

DEFINES := \
	-DSTM32H743xx

INCLUDES := \
	-Isrc \
	-Itarget/DELTAH743 \
	-Ilib/CMSIS/Core/Include \
	-Ilib/CMSIS/Device/ST/STM32H7xx/Include \
	-Isrc/drivers \
	-Isystem

CFLAGS := \
	$(ARCH_FLAGS) \
	$(DEFINES) \
	$(INCLUDES) \
	-std=gnu11 \
	-Og \
	-g3 \
	-ffunction-sections \
	-fdata-sections \
	-fno-common \
	-Wall \
	-Wextra \
	-Wshadow \
	-Wundef \
	-MMD \
	-MP

ASFLAGS := \
	$(ARCH_FLAGS) \
	$(DEFINES) \
	$(INCLUDES) \
	-x assembler-with-cpp \
	-g3

LDFLAGS := \
	$(ARCH_FLAGS) \
	-nostartfiles \
	-Tlinker/stm32h743_2m.ld \
	-Wl,--gc-sections \
	-Wl,-Map=$(BUILD_DIR)/$(TARGET).map \
	-Wl,--print-memory-usage \
	--specs=nano.specs \
	--specs=nosys.specs

C_SOURCES := \
	src/main.c \
	src/drivers/uart.c \
	system/system_stm32h743xx.c 

ASM_SOURCES := \
	startup/startup_stm32h743xx.s

C_OBJECTS := \
	$(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

ASM_OBJECTS := \
	$(patsubst %.s,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)
DEPS := $(C_OBJECTS:.o=.d)

ELF := $(BUILD_DIR)/$(TARGET).elf
HEX := $(BUILD_DIR)/$(TARGET).hex
BIN := $(BUILD_DIR)/$(TARGET).bin

.PHONY: all clean flash size compile_commands

all: $(ELF) $(HEX) $(BIN)
	$(SIZE) $(ELF)

$(ELF): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

size: $(ELF)
	$(SIZE) $(ELF)

flash: $(ELF)
	openocd \
		-f interface/stlink.cfg \
		-f target/stm32h7x.cfg \
		-c "program $(ELF) verify reset exit"

compile_commands:
	rm -f compile_commands.json
	bear -- $(MAKE) clean all

clean:
	rm -rf build

-include $(DEPS)