__und_stack_size__  = 0x0400;
__abt_stack_size__  = 0x0400;
__fiq_stack_size__  = 0x0400;
__irq_stack_size__  = 0x0400;
__svc_stack_size__  = 0x0400;
__sys_stack_size__  = 0x0400;
__stacks_total_size__   = __und_stack_size__ + __abt_stack_size__ + __fiq_stack_size__ + __irq_stack_size__ + __svc_stack_size__ + __sys_stack_size__;

MEMORY
{
        SYSTEM_RAM :  org = 0x200000, len = 0x06000000
}

__ram_start__   = ORIGIN(SYSTEM_RAM);
__ram_size__    = LENGTH(SYSTEM_RAM);
__ram_end__     = __ram_start__ + __ram_size__;

SECTIONS {
    __sys_start__  = __ram_start__;
    .init : {
        *(.init)
    } > SYSTEM_RAM

	.text ALIGN(0x10): {
        __text_start__  = .;
		*(.text)
        *(.rodata)
        __text_end__    = .;
	} > SYSTEM_RAM


	.data ALIGN(0x10): {
        __data_start__  = .;
		*(.data)
        __data_end__    = .;
	} > SYSTEM_RAM

    .bss ALIGN(0x10): {
        __bss_start__   = .;
        *(.bss)
        *(COMMON)
        __bss_end__     = .;

        __stack_start__ = .;
        . = . + 0x20000; /* 128K for int stack */
        __stack_end__   = .;
    } > SYSTEM_RAM

    __sys_end__ = .;
}
