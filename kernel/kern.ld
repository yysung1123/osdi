/* Link kernel as a ELF file .
   See the GNU ld 'info' manual ("info ld") to learn the syntax. */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)
kernel_load_addr = 0xF0100000;

SECTIONS
{
	. = kernel_load_addr;
	PROVIDE(_FOOTPRINT_START_ = .);

	/* AT(...) gives the load address of this section, which tells
	   the boot loader where to load the kernel in physical memory */
	.text : AT(0x100000) {
		PROVIDE(_TEXT_START_ = .);
		*(.text .stub .text.* .gnu.linkonce.t.*)
		PROVIDE(_TEXT_END_ = .);
	}
	PROVIDE(etext = .);	/* Define the 'etext' symbol to this value */

	.rodata : {
		*(.rodata .rodata.* .gnu.linkonce.r.*)
	}

	/* Include debugging information in kernel memory */
	.stab : {
		PROVIDE(__STAB_BEGIN__ = .);
		*(.stab);
		PROVIDE(__STAB_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}

	.stabstr : {
		PROVIDE(__STABSTR_BEGIN__ = .);
		*(.stabstr);
		PROVIDE(__STABSTR_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}
	
	/* Adjust the address for the data segment to the next page */
	. = ALIGN(0x1000);

	/* The data segment */
	.data : {
	        PROVIDE(_DATA_START_ = .);
		*(.data)
	}
	.bss : {
	        PROVIDE(_BSS_START_ = .);
	        *(.dynbss)
                *(.bss .bss.*)
		*(COMMON)
	        PROVIDE(_DATA_END_ = .);
	        PROVIDE(end = .);
	}

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack)
	}
	PROVIDE(_FOOTPRINT_END_ = .);
}
