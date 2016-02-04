__und_stack_size__  = 0x0400;
__abt_stack_size__  = 0x0400;
__fiq_stack_size__  = 0x0400;
__irq_stack_size__  = 0x0400;
__svc_stack_size__  = 0x0400;
__sys_stack_size__  = 0x0400;
__stacks_total_size__   = __und_stack_size__ + __abt_stack_size__ + __fiq_stack_size__ + __irq_stack_size__ + __svc_stack_size__ + __sys_stack_size__;

/*MEMORY
{
        SYSTEM_RAM : ORIGIN = 0x8000, LENGTH = 0x06000000 - 0x20
}*/

__ram_start__ = 0x8000;
__ram_end__   = 0x8000 + 0x06000000 - 0x20;

SECTIONS {
    __sys_start__  = __ram_start__;
    .init 0x8000 : {
        *(.init)
    } 

	.text ALIGN(0x10): {
        __text_start__ = .;
		*(.text)
        *(.rodata)
        __text_end__   = .;
	}


	.data ALIGN(0x10): {
        __data_start__ = .;
		*(.data)
        __data_end__  = .;
	}

    .bss ALIGN(0x10): {
        __bss_start__ = .;
        *(.bss)
        *(COMMON)
        __bss_end__ = .;
    }

    __sys_end__ = .;
}
