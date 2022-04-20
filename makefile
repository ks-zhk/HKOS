BUILD_DIR = ./build
ENTRY_POINT = 0xc0001500
AS = nasm
CC = gcc
LD = ld
LIB = -I lib/ -I lib/kernel/ -I lib/user/ -I device/ -I thread/ -I device/c/
ASFLAGS = -f elf
CFLAGS = -m32 -Wall $(LIB) -c -fno-builtin -W -Wstrict-prototypes \
		 -Wmissing-prototypes
LDFLAGS = -Ttext $(ENTRY_POINT) -e main -m elf_i386
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/console.o $(BUILD_DIR)/sync.o $(BUILD_DIR)/thread.o $(BUILD_DIR)/init.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/bitmap.o $(BUILD_DIR)/string.o $(BUILD_DIR)/debug.o $(BUILD_DIR)/interrupt.o $(BUILD_DIR)/print.o \
	   $(BUILD_DIR)/timer.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/switch.o $(BUILD_DIR)/list.o

###### compile c file #########
$(BUILD_DIR)/main.o: lib/kernel/main.c lib/stdint.h lib/kernel/print.h lib/kernel/init.h lib/kernel/debug.h \
lib/kernel/memory.h thread/thread.h lib/interrupt.h device/c/console.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/init.o: lib/kernel/init.c lib/kernel/init.h lib/stdint.h lib/kernel/print.h lib/interrupt.h \
device/c/timer.h thread/thread.h device/c/console.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/debug.o: lib/kernel/debug.c lib/kernel/debug.h lib/kernel/print.h lib/interrupt.h \
lib/stdint.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/interrupt.o: lib/kernel/interrupt.c lib/interrupt.h lib/stdint.h lib/kernel/print.h \
lib/kernel/io.h lib/kernel/global.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/timer.o: device/c/timer.c device/c/timer.h lib/stdint.h lib/kernel/io.h lib/kernel/print.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/string.o: lib/kernel/string.c lib/kernel/string.h lib/stdint.h lib/kernel/debug.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/bitmap.o: lib/kernel/bitmap.c lib/kernel/bitmap.h lib/stdint.h lib/interrupt.h \
lib/kernel/string.h lib/kernel/global.h lib/kernel/debug.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/memory.o: lib/kernel/memory.c lib/kernel/memory.h lib/kernel/print.h lib/kernel/string.h \
lib/kernel/bitmap.h lib/stdint.h lib/kernel/debug.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/thread.o:	thread/thread.c thread/thread.h lib/stdint.h lib/kernel/memory.h lib/kernel/global.h lib/kernel/string.h \
lib/interrupt.h lib/kernel/list.h  lib/kernel/debug.h lib/kernel/print.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/list.o:	lib/kernel/list.c lib/kernel/list.h lib/stdint.h lib/interrupt.h lib/kernel/debug.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/sync.o:	thread/sync.c thread/sync.h thread/thread.h lib/stdint.h lib/interrupt.h lib/kernel/debug.h lib/kernel/list.h
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/console.o:	device/c/console.c device/c/console.h thread/thread.h thread/sync.h lib/stdint.h lib/kernel/print.h
	$(CC) $(CFLAGS) $< -o $@
###### compile asm file ########
$(BUILD_DIR)/print.o: lib/kernel/print.S
	$(AS) $(ASFLAGS) $< -o $@
$(BUILD_DIR)/kernel.o: lib/kernel/kernel.S
	$(AS) $(ASFLAGS) $< -o $@
$(BUILD_DIR)/switch.o: thread/switch.S
	$(AS) $(ASFLAGS) $< -o $@

####### link all obj file #######
$(BUILD_DIR)/kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

.PHONY : hd clean all

hd:
	dd if=$(BUILD_DIR)/kernel.bin \
	   of=/home/ksdeer/apps/bochs/bochs_2_6_2/bin/hd60M.img \
	   bs=512 count=200 seek=9 conv=notrunc
clean:
	cd $(BUILD_DIR) && rm -f ./*
build:$(BUILD_DIR)/kernel.bin

all:build hd
	@echo "build ok"

