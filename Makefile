############ by wuxx #############

CFLAGS = $(cflags)

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

KERNEL_DIR  = $(ROOT)/kernel
SYSTEM_DIR  = $(ROOT)/system
LIBC_DIR    = $(ROOT)/libc
DRIVER_DIR  = $(ROOT)/driver
TEST_DIR    = $(ROOT)/test
INCLUDE_DIR = $(ROOT)/include

KERNEL_SRCS = \
		$(KERNEL_DIR)/kernel.c  \
		$(KERNEL_DIR)/os_task.c

SYSTEM_SRCS = \
		$(SYSTEM_DIR)/main.c 	\
		$(SYSTEM_DIR)/int.c 	\
		$(SYSTEM_DIR)/arm_v6.s 	\
		$(SYSTEM_DIR)/init.s

LIBC_SRCS = \
		$(LIBC_DIR)/printf.c	\
		$(LIBC_DIR)/string.c	\
		$(LIBC_DIR)/vsprintf.c	

DRIVER_SRCS = \
		$(DRIVER_DIR)/gpio/gpio.c       \
		$(DRIVER_DIR)/timer/timer.c     \
		$(DRIVER_DIR)/log/log.c         \
		$(DRIVER_DIR)/shell/shell.c     \
		$(DRIVER_DIR)/uart/uart.c

TEST_SRCS = \
		$(TEST_DIR)/systest.c           \
		$(TEST_DIR)/test_libc.c         \
		$(TEST_DIR)/test_cpu.c          \
		$(TEST_DIR)/test_timer.c        \
		$(TEST_DIR)/test_log.c          \
		$(TEST_DIR)/test_gpio.c

ALL_SRCS = $(KERNEL_SRCS) $(SYSTEM_SRCS) $(LIBC_SRCS) $(DRIVER_SRCS) $(TEST_SRCS)

C_SRCS   = $(filter %.c, $(ALL_SRCS))
ASM_SRCS = $(filter %.s, $(ALL_SRCS)) 
H_SRCS   = $(wildcard $(INCLUDE_DIR)/*.h)

C_OBJS   = $(addprefix $(BUILD)/, $(patsubst %.c,%.o,$(C_SRCS)))
ASM_OBJS = $(addprefix $(BUILD)/, $(patsubst %.s,%.o,$(ASM_SRCS)))

ALL_OBJS = $(C_OBJS) $(ASM_OBJS)
#$(warning ALL_SRCS $(ALL_SRCS))
#$(warning ALL_OBJS $(ALL_OBJS))

#$(warning C_SRCS $(C_SRCS) ASM_SRCS $(ASM_SRCS))
#$(warning C_OBJS $(C_OBJS) ASM_OBJS $(ASM_OBJS))


OBJ_PATHS = $(addprefix $(BUILD)/, $(sort $(dir $(ALL_SRCS))))
#$(warning OBJ_PATHS $(OBJ_PATHS))


TARGET = sos
TARGET_ELF = $(BUILD)/$(TARGET).elf
TARGET_IMG = $(BUILD)/$(TARGET).img
TARGET_MAP = $(BUILD)/$(TARGET).map
TARGET_DISASM = $(BUILD)/$(TARGET).disasm
TARGET_ELFINFO = $(BUILD)/$(TARGET).elfinfo
TARGET_SECINFO = $(BUILD)/$(TARGET).secinfo #section info

LDS = $(ROOT)/$(TARGET).ld

#-march=armv6
CFLAGS  += -mcpu=arm1176jzf-s -fno-builtin -mno-thumb-interwork -fomit-frame-pointer -I$(INCLUDE_DIR)
ASFLAGS += 

LIBGCC = $(shell find $(TOOLCHAIN_DIR)/ | grep "armv6-m\/libgcc\.a")
LDFLAGS = -T $(LDS) -Map $(TARGET_MAP) -nostdlib -nostartfiles $(LIBGCC) 

.PHONY: build_all clean tags

build_all: all

$(C_OBJS): $(H_SRCS)
$(C_OBJS): $(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM_OBJS): $(BUILD)/%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

build_objs: $(C_OBJS) $(ASM_OBJS)

init:
	mkdir -p build
	$(foreach d,$(OBJ_PATHS), mkdir -p $(d);)

all:init build_objs
	$(LD) $(ALL_OBJS) $(LDFLAGS) -o $(TARGET_ELF)
	$(OBJCOPY) $(TARGET_ELF) -O binary $(TARGET_IMG)
	$(OBJDUMP) -d $(TARGET_ELF) > $(TARGET_DISASM)
	$(OBJDUMP) -s $(TARGET_ELF) > $(TARGET_SECINFO)
	$(READELF) -a $(TARGET_ELF) > $(TARGET_ELFINFO)
	cp $(TARGET_IMG) $(BUILD)/kernel.img
	echo "CFLAGS: $(CFLAGS)"

tags:
	ctags -R $(KERNEL_DIR) $(SYSTEM_DIR) $(LIBC_DIR) $(DRIVER_DIR) $(TEST_DIR) $(INCLUDE_DIR)
	#find ./driver ./libc ./include ./system ./test | grep ".*\.[ch]$$" | xargs wc -l


clean: 
	-rm -rf build
