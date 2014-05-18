###############################################################################
#	makefile
#	 by Alex Chadwick
#
#	A makefile script for generation of raspberry pi kernel images.
###############################################################################

# The toolchain to use. arm-none-eabi works, but there does exist 
# arm-bcm2708-linux-gnueabi.
ARMGNU ?= arm-none-eabi

# The intermediate directory for compiled object files.
BUILD = build/

INCLUDE = ./lib
C_SRCS =  ./main.c ./lib/vsprintf.c 
ASM_SRC_FILES = ./arm_v6.c ./setup.s 

# The name of the output file to generate.
TARGET = kernel.img

# The name of the assembler listing file to generate.
LIST = kernel.list

# The name of the map file to generate.
MAP = kernel.map

# The name of the linker script to use.
LINKER = kernel.ld

DISASM = out.s
#-march=armv6
CFLAGS = -mcpu=arm1176jzf-s -fno-builtin -I$(INCLUDE)

LIB = /home/wuxx/arm-2008q3/lib/gcc/arm-none-eabi/4.3.2/armv6-m/libgcc.a
LDFLAGS = -nostdlib -nostartfiles -T $(LINKER) -Map $(MAP) $(LIB)

all: 
	$(ARMGNU)-as setup.s -o setup.o
	$(ARMGNU)-as arm_v6.s -o arm_v6.o
	$(ARMGNU)-gcc -c ./lib/vsprintf.c $(CFLAGS)
	$(ARMGNU)-gcc -c main.c $(CFLAGS) -o main.o
	$(ARMGNU)-ld setup.o arm_v6.o vsprintf.o main.o $(LDFLAGS) -o output.elf
	$(ARMGNU)-objcopy output.elf -O binary $(TARGET)
	$(ARMGNU)-objdump -d output.elf > $(DISASM)

clean: 
	cd ./lib; make clean
	-rm -rf *.o *.elf $(MAP) $(TARGET) $(DISASM)
