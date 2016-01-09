###############################################################################
#	makefile
#	 by Alex Chadwick
#
#	A makefile script for generation of raspberry pi kernel images.
###############################################################################

# The toolchain to use. arm-none-eabi works, but there does exist 
# arm-bcm2708-linux-gnueabi.
ROOT = $(shell pwd)
$(warning ROOT $(ROOT))
TOOLCHAIN_DIR=$(ROOT)/.compiler
CROSS_COMPILE=$(TOOLCHAIN_DIR)/bin/arm-none-eabi

CC = $(CROSS_COMPILE)-gcc
AS = $(CROSS_COMPILE)-as
LD = $(CROSS_COMPILE)-ld
OBJCOPY = $(CROSS_COMPILE)-objcopy
OBJDUMP = $(CROSS_COMPILE)-objdump
READELF = $(CROSS_COMPILE)-readelf

# The intermediate directory for compiled object files.
BUILD = $(ROOT)/build/

C_SRCS =  ./system/main.c ./libc/vsprintf.c 
ASM_SRC_FILES = ./system/arm_v6.c ./system/setup.s 

SYSTEM_DIR  = $(ROOT)/system
LIBC_DIR    = $(ROOT)/libc
DRIVER_DIR  = $(ROOT)/driver
INCLUDE_DIR = $(ROOT)/include

SYSTEM_SRCS = \
			  $(SYSTEM_DIR)/main.c 		\
			  $(SYSTEM_DIR)/arm_v6.c 	\
			  $(SYSTEM_DIR)/setup.s

LIBC_SRCS = \
			$(LIBC_DIR)/printf.c	\
			$(LIBC_DIR)/vsprintf.c	

DRIVER_SRCS = \
		$(DRIVER_DIR)/uart/uart.c

# The name of the output file to generate.
TARGET_ELF = kernel.elf
TARGET_BIN = kernel.bin
TARGET_MAP = kernel.map
TARGET_DISASM = kernel.disasm
TARGET_ELFINFO = kernel.elfinfo

LDS = kernel.ld

DISASM = kernel.disasm
#-march=armv6
CFLAGS = -mcpu=arm1176jzf-s -fno-builtin -I$(INCLUDE_DIR)
ASFLAGS = 

LIBGCC = $(TOOLCHAIN_DIR)/lib/gcc/arm-none-eabi/4.3.2/armv6-m/libgcc.a
LDFLAGS = -T $(LDS) -Map $(TARGET_MAP) -nostdlib -nostartfiles $(LIBGCC) 

all: 
	$(AS) $(ASFLAGS) ./system/setup.s -o setup.o
	$(AS) $(ASFLAGS) ./system/arm_v6.s -o arm_v6.o
	$(CC) $(CFLAGS) -c ./libc/vsprintf.c 
	$(CC) $(CFLAGS) -c ./system/main.c  -o main.o
	$(LD)  setup.o arm_v6.o vsprintf.o main.o $(LDFLAGS) -o $(TARGET_ELF)
	$(OBJCOPY) kernel.elf -O binary $(TARGET_BIN)
	$(OBJDUMP) -d kernel.elf > $(TARGET_DISASM)
	$(READELF) -a kernel.elf > $(TARGET_ELFINFO)

clean: 
	-rm -rf *.o *.bin *.elf* $(MAP) $(TARGET) $(DISASM)
