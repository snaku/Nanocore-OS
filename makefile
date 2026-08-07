CC = x86_64-elf-gcc
LD = x86_64-elf-ld
OBJCOPY = x86_64-elf-objcopy
NASM = nasm

CFLAGS = -std=gnu11 \
	-ffreestanding \
	-fno-tree-loop-distribute-patterns \
	-fno-stack-protector \
	-fno-builtin \
	-fno-pic \
	-fno-pie \
	-mno-red-zone \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mcmodel=small \
	-Wall \
	-Wextra \
	-Iinclude

LDFLAGS = -T linker.ld \
	-nostdlib \
	-z max-page-size=0x1000

BUILD = build

BOOT1 = $(BUILD)/boot_1.bin
BOOT2 = $(BUILD)/boot_2.bin

ENTRY_OBJ = $(BUILD)/kernel/entry.o

C_SRCS = $(shell find src -type f -name '*.c')
C_OBJS = $(patsubst src/%.c,$(BUILD)/%.o,$(C_SRCS))

OBJS = $(C_OBJS)

KERNEL_ELF = $(BUILD)/kernel.elf
KERNEL_BIN = $(BUILD)/kernel.bin
IMG = $(BUILD)/ncOS.img

all: $(IMG)

$(BUILD):
	mkdir -p $(BUILD)

# bootloader

$(BOOT1): src/boot/boot_1.asm | $(BUILD)
	$(NASM) -f bin $< -o $@

$(BOOT2): src/boot/boot_2.asm | $(BUILD)
	$(NASM) -f bin $< -o $@

# kernel entry

$(ENTRY_OBJ): src/kernel/entry.asm
	mkdir -p $(dir $@)
	$(NASM) -f elf64 $< -o $@

$(BUILD)/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# kernel

$(KERNEL_ELF): $(ENTRY_OBJ) $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

# create img

$(IMG): $(BOOT1) $(BOOT2) $(KERNEL_BIN)
	cat $(BOOT1) $(BOOT2) $(KERNEL_BIN) > $@
	truncate -s 1M $@

clean:
	rm -rf $(BUILD)

.PHONY: all clean