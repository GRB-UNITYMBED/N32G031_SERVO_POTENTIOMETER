######################################
# Toolchain Setup
######################################
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

######################################
# Project Files
######################################
TARGET = My_Project
OUTDIR = build

# Source Files
SRCS = src/main.c \
       startup/startup_n32g031_gcc.s \
       src/system_n32g031.c \
       drivers/src/n32g031_rcc.c \
       drivers/src/n32g031_usart.c \
       drivers/src/n32g031_gpio.c \
       drivers/src/n32g031_adc.c

# Include Paths (Update these to match your folder structure)
INCLUDES = -I. -Iinc -IN32_SDK -ICMSIS/Core/Include -Idrivers/inc -IN32G031_StdPeriph_Driver/inc

######################################
# Flags
######################################
# MCU Specific Flags
MCU = -mcpu=cortex-m0 -mthumb

# Compiler Flags
CFLAGS = $(MCU) -O0 -g -Wall $(INCLUDES) \
         -ffunction-sections -fdata-sections

# Linker Flags
LDSCRIPT = n32g031_flash.ld
LDFLAGS = $(MCU) -T$(LDSCRIPT) \
          -Wl,--gc-sections \
          --specs=nosys.specs \
          -Wl,-Map=$(OUTDIR)/$(TARGET).map

######################################
# Build Rules
######################################

# Convert source list to object list in build directory
OBJS = $(addprefix $(OUTDIR)/, $(addsuffix .o, $(basename $(SRCS))))

all: $(OUTDIR)/$(TARGET).hex

# Rule to create Hex from Elf
$(OUTDIR)/$(TARGET).hex: $(OUTDIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@
	$(SIZE) $<

# Rule to Link Elf
$(OUTDIR)/$(TARGET).elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# Rule for C files
$(OUTDIR)/%.o: %.c
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for Assembly files
$(OUTDIR)/%.o: %.s
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR):
	mkdir -p $(OUTDIR)

######################################
# Flash and Debug
######################################
OPENOCD_BIN = /C/openocd-v0.12/bin/openocd.exe

flash: all
	$(OPENOCD_BIN) \
	-f interface/cmsis-dap.cfg \
	-f target/n32g03x.cfg \
	-c "adapter speed 1000" \
	-c "reset_config none" \
	-c "init" \
	-c "halt" \
	-c "flash write_image erase $(OUTDIR)/$(TARGET).elf" \
	-c "verify_image $(OUTDIR)/$(TARGET).elf" \
	-c "reset run" \
	-c "exit"

clean:
	rm -rf $(OUTDIR)

.PHONY: all clean flash
