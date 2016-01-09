############ by wuxx #############

ROOT = $(shell pwd)
#$(warning ROOT $(ROOT))
TOOLCHAIN_DIR=$(ROOT)/.compiler
CROSS_COMPILE=$(TOOLCHAIN_DIR)/bin/arm-none-eabi

CC = $(CROSS_COMPILE)-gcc
AS = $(CROSS_COMPILE)-as
LD = $(CROSS_COMPILE)-ld
OBJCOPY = $(CROSS_COMPILE)-objcopy
OBJDUMP = $(CROSS_COMPILE)-objdump
READELF = $(CROSS_COMPILE)-readelf

BUILD = $(ROOT)/build


SYSTEM_DIR  = $(ROOT)/system
LIBC_DIR    = $(ROOT)/libc
DRIVER_DIR  = $(ROOT)/driver
INCLUDE_DIR = $(ROOT)/include

SYSTEM_SRCS = \
		$(SYSTEM_DIR)/main.c 	\
		$(SYSTEM_DIR)/arm_v6.s 	\
		$(SYSTEM_DIR)/init.s

LIBC_SRCS = \
		$(LIBC_DIR)/printf.c	\
		$(LIBC_DIR)/vsprintf.c	

DRIVER_SRCS = \
		 #$(DRIVER_DIR)/uart/uart.c

ALL_SRCS = $(SYSTEM_SRCS) $(LIBC_SRCS) $(DRIVER_SRCS)

C_SRCS   = $(filter %.c, $(ALL_SRCS))
ASM_SRCS = $(filter %.s, $(ALL_SRCS)) 

C_OBJS   = $(patsubst %.c,%.o,$(C_SRCS))
ASM_OBJS = $(patsubst %.s,%.o,$(ASM_SRCS))

$(warning C_SRCS $(C_SRCS) ASM_SRCS $(ASM_SRCS))
$(warning C_OBJS $(C_OBJS) ASM_OBJS $(ASM_OBJS))


OBJ_PATHS = $(sort $(dir $(ALL_SRCS)))
#$(warning OBJ_PATHS $(OBJ_PATHS))



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


$(C_OBJS): %.o: %.c
	echo hello,world
	echo $< 111 $@
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM_OBJS): %.o: %.s
	 $(AS) $(ASFLAGS) $< -o $@

test: $(C_OBJS) $(ASM_OBJS)
	echo hello,world

all: 
	$(AS) $(ASFLAGS) ./system/init.s -o init.o
	$(AS) $(ASFLAGS) ./system/arm_v6.s -o arm_v6.o
	$(CC) $(CFLAGS) -c ./libc/vsprintf.c 
	$(CC) $(CFLAGS) -c ./system/main.c  -o main.o
	$(LD)  init.o arm_v6.o vsprintf.o main.o $(LDFLAGS) -o $(TARGET_ELF)
	$(OBJCOPY) kernel.elf -O binary $(TARGET_BIN)
	$(OBJDUMP) -d kernel.elf > $(TARGET_DISASM)
	$(READELF) -a kernel.elf > $(TARGET_ELFINFO)

clean: 
	-rm -rf *.o *.bin *.elf* $(MAP) $(TARGET) $(DISASM)
