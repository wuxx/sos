###############################################################################
#	makefile
#	 by Alex Chadwick
#
#	A makefile script for generation of raspberry pi kernel images.
###############################################################################

# The toolchain to use. arm-none-eabi works, but there does exist 
# arm-bcm2708-linux-gnueabi.
TOOLCHAIN_DIR=.compiler

CC = $(TOOLCHAIN_DIR)/bin/arm-none-eabi-gcc
AS = $(TOOLCHAIN_DIR)/bin/arm-none-eabi-as
LD = $(TOOLCHAIN_DIR)/bin/arm-none-eabi-ld
OBJCOPY = $(TOOLCHAIN_DIR)/bin/arm-none-eabi-objcopy
OBJDUMP = $(TOOLCHAIN_DIR)/bin/arm-none-eabi-objdump
READELF = $(TOOLCHAIN_DIR)/bin/arm-none-eabi-readelf

# The intermediate directory for compiled object files.
BUILD = build/

ROOT = .
C_SRCS =  ./system/main.c ./lib/vsprintf.c 
ASM_SRC_FILES = ./system/arm_v6.c ./system/setup.s 

DRIVER_DIR  = $(ROOT)/driver
INCLUDE_DIR = ./include

DRIVER_SRCS = \
		$(DRIVER_DIR)/uart/uart.c

# The name of the output file to generate.
TARGET = kernel.img

# The name of the map file to generate.
MAP = kernel.map

# The name of the linker script to use.
LINKER = kernel.ld

DISASM = kernel.disasm
#-march=armv6
CFLAGS = -mcpu=arm1176jzf-s -fno-builtin -I$(INCLUDE_DIR)

LIB = $(TOOLCHAIN_DIR)/lib/gcc/arm-none-eabi/4.3.2/armv6-m/libgcc.a
LDFLAGS = -nostdlib -nostartfiles -T $(LINKER) -Map $(MAP) $(LIB)

all: 
	$(AS) ./system/setup.s -o setup.o
	$(AS) ./system/arm_v6.s -o arm_v6.o
	$(CC) -c ./lib/vsprintf.c $(CFLAGS)
	$(CC) $(CFLAGS) -c ./system/main.c  -o main.o
	$(LD) setup.o arm_v6.o vsprintf.o main.o $(LDFLAGS) -o kernel.elf
	$(OBJCOPY) kernel.elf -O binary $(TARGET)
	$(OBJDUMP) -d kernel.elf > $(DISASM)
	$(READELF) -a kernel.elf > kernel.elfinfo

clean: 
	cd ./lib; make clean
	-rm -rf *.o *.elf* $(MAP) $(TARGET) $(DISASM)
