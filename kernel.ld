
SECTIONS {
    __sys_start__ = .;
	.init 0x8000 : {
        KEEP(*(vector_table))
		*(.init)
	}
	
    __text_start__ = .;
	.text : {
		*(.text)
        *(.rodata)
	}
    __text_end__   = .;

    __data_start__ = .;
	.data : {
		*(.data)
	}
    __data_end__  = .;

    __bss_start__ = .;
    .bss : {
        *(.bss)
        *(COMMON)
    }
    __bss_end__ = .;

    __sys_end__ = .;
	/*
	* Finally comes everything else. A fun trick here is to put all other 
	* sections into this section, which will be discarded by default.
	*/
	/DISCARD/ : {
		*(*)
	}
}
