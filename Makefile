############ by wuxx #############

#ROOT = $(shell pwd)
ROOT = .
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
		$(SYSTEM_DIR)/int.c 	\
		$(SYSTEM_DIR)/arm_v6.s 	\
		$(SYSTEM_DIR)/init.s

LIBC_SRCS = \
		$(LIBC_DIR)/printf.c	\
		$(LIBC_DIR)/vsprintf.c	

DRIVER_SRCS = \
		$(DRIVER_DIR)/gpio/gpio.c       \
		$(DRIVER_DIR)/timer/timer.c     \
		$(DRIVER_DIR)/log/log.c         \
		$(DRIVER_DIR)/uart/uart.c

ALL_SRCS = $(SYSTEM_SRCS) $(LIBC_SRCS) $(DRIVER_SRCS)

C_SRCS   = $(filter %.c, $(ALL_SRCS))
ASM_SRCS = $(filter %.s, $(ALL_SRCS)) 

C_OBJS   = $(patsubst %.c,%.o,$(C_SRCS))
ASM_OBJS = $(patsubst %.s,%.o,$(ASM_SRCS))

ALL_OBJS = $(addprefix $(BUILD)/,$(C_OBJS) $(ASM_OBJS))
#$(warning ALL_SRCS $(ALL_SRCS))
#$(warning ALL_OBJS $(ALL_OBJS))

#$(warning C_SRCS $(C_SRCS) ASM_SRCS $(ASM_SRCS))
#$(warning C_OBJS $(C_OBJS) ASM_OBJS $(ASM_OBJS))


OBJ_PATHS = $(sort $(dir $(ALL_SRCS)))
#$(warning OBJ_PATHS $(OBJ_PATHS))



TARGET_ELF = $(BUILD)/kernel.elf
TARGET_IMG = $(BUILD)/kernel.img
TARGET_MAP = $(BUILD)/kernel.map
TARGET_DISASM = $(BUILD)/kernel.disasm
TARGET_ELFINFO = $(BUILD)/kernel.elfinfo

LDS = kernel.ld

DISASM = kernel.disasm
#-march=armv6
CFLAGS = -mcpu=arm1176jzf-s -fno-builtin -I$(INCLUDE_DIR)
ASFLAGS = 

LIBGCC = $(TOOLCHAIN_DIR)/lib/gcc/arm-none-eabi/4.3.2/armv6-m/libgcc.a
LDFLAGS = -T $(LDS) -Map $(TARGET_MAP) -nostdlib -nostartfiles $(LIBGCC) 

build_all: all

$(C_OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $(BUILD)/$@

$(ASM_OBJS): %.o: %.s
	$(AS) $(ASFLAGS) $< -o $(BUILD)/$@

build_objs: $(C_OBJS) $(ASM_OBJS)

init:
	mkdir -p build
	$(foreach d,$(OBJ_PATHS), mkdir -p $(BUILD)/$(d);)

all:init build_objs
	$(LD) $(ALL_OBJS) $(LDFLAGS) -o $(TARGET_ELF)
	$(OBJCOPY) $(TARGET_ELF) -O binary $(TARGET_IMG)
	$(OBJDUMP) -d $(TARGET_ELF) > $(TARGET_DISASM)
	$(READELF) -a $(TARGET_ELF) > $(TARGET_ELFINFO)

tags:
	ctags -R ./drivers ./libc ./include ./system

clean: 
	-rm -rf build
