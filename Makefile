TARGET = firmware
BUILD_DIR = build

# Source files
C_SOURCES = \
src/main.c \
src/n32g031_it.c \
src/servo.c \
src/system_n32g031.c \
src/utils.c \
$(wildcard drivers/src/*.c)

ASM_SOURCES = \
startup/startup_n32g031_gcc.s

# Toolchain
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

# MCU flags for N32G031 (Cortex-M0)
MCU = -mcpu=cortex-m0 -mthumb

# Includes
C_INCLUDES = \
-Iinc \
-Idrivers/inc

# Compile flags
CFLAGS = $(MCU) -O2 -g -Wall -fdata-sections -ffunction-sections $(C_INCLUDES)
ASFLAGS = $(MCU) -O2 -g -Wall -fdata-sections -ffunction-sections

# Linker flags
LDSCRIPT = n32g031_flash.ld
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# Object files (preserving directory structure in build folder)
OBJECTS = $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES:.s=.o))

# Default target
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# Build rules
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(@D)
	$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/$(TARGET).elf
	$(CP) -O ihex $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/$(TARGET).elf
	$(CP) -O binary -S $< $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
