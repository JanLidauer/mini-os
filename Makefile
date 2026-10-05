TARGET    := i686-elf
CC        := $(TARGET)-gcc
AS        := $(TARGET)-as

SRC_DIR   := src
BUILD_DIR := build
ISO_DIR   := $(BUILD_DIR)/isodir

CFLAGS    := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude
LDFLAGS   := -ffreestanding -O2 -nostdlib

KERNEL    := $(BUILD_DIR)/myos.bin
ISO       := $(BUILD_DIR)/myos.iso

.PHONY: all iso run clean docker-build docker-run

all: $(KERNEL)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

OBJS      := $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/gdt.o $(BUILD_DIR)/gdt_flush.o

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s | $(BUILD_DIR)
	$(AS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

$(KERNEL): $(SRC_DIR)/linker.ld $(OBJS)
	$(CC) -T $(SRC_DIR)/linker.ld -o $@ $(LDFLAGS) $(OBJS) -lgcc

iso: $(KERNEL)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL) $(ISO_DIR)/boot/myos.bin
	cp grub/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(ISO_DIR)

# -display curses: rendert den VGA-Textmodus direkt im Terminal, weil im
# Docker-Container/WSL kein X11-Display fuer die GTK-Ausgabe vorhanden ist.
run: iso
	qemu-system-i386 -cdrom $(ISO) -display curses

clean:
	rm -rf $(BUILD_DIR)

# Diese Targets laufen am Host und stossen den Container an, in dem der
# Cross-Compiler/GRUB/QEMU installiert sind (siehe docker/Dockerfile).
docker-build:
	./docker/build.sh

docker-run: docker-build
	docker run --rm -it -v $(CURDIR):/root/mini-os mini-os make run
