/* Link kernel as a ELF file .
See the GNU ld 'info' manual ("info ld") to learn the syntax. */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)
kernel_load_addr = 0xF0100000;

SECTIONS
{
. = kernel_load_addr;

  /* AT(...) gives the load address of this section, which tells
  the boot loader where to load the kernel in physical memory */
. = ALIGN(0x1000);
PROVIDE(UTEXT_start = .);
PROVIDE(stext = .);
.text : AT(0x100000) {
  lib/string.o (.text)
  lib/printf.o (.text)
  lib/printfmt.o (.text)
  lib/readline.o (.text)
  lib/console.o (.text)
  lib/syscall.o (.text)
  user/shell.o (.text)
  user/main.o (.text)
  /usr/lib/gcc/x86_64-pc-linux-gnu/8.3.0/32/libgcc.a (.*)
  PROVIDE(UTEXT_end = .);
  *(.text .stub .text.* .gnu.linkonce.t.*)
}
PROVIDE(etext = .);	/* Define the 'etext' symbol to this value */

. = ALIGN(0x1000);
PROVIDE(URODATA_start = .);
.rodata : {
  lib/string.o (.rodata)
  lib/printf.o (.rodata)
  lib/printfmt.o (.rodata)
  lib/readline.o (.rodata)
  lib/console.o (.rodata)
  lib/syscall.o (.rodata)
  user/shell.o (.rodata)
  user/main.o (.rodata)
PROVIDE(URODATA_end = .);
  *(.rodata .rodata.* .gnu.linkonce.r.*)
  *(.data.rel.ro.local)
}
PROVIDE(rdata_end = .);

/* Adjust the address for the data segment to the next page */
. = ALIGN(0x1000);

/* The data segment */
PROVIDE(data_start = .);
.data : {
PROVIDE(UDATA_start = .);
  lib/string.o (.data .data.*)
  lib/printf.o (.data .data.*)
  lib/printfmt.o (.data .data.*)
  lib/readline.o (.data .data.*)
  lib/console.o (.data .data.*)
  lib/syscall.o (.data .data.*)
  user/shell.o (.data .data.*)
  user/main.o (.data .data.*)
PROVIDE(UDATA_end = .);
  *(.data)
  *(.data.rel.local)
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

PROVIDE(bss_start = .);
.bss : {
PROVIDE(UBSS_start = .);
  lib/string.o (.bss .bss.*)
  lib/printf.o (.bss .bss.*)
  lib/printfmt.o (.bss .bss.*)
  lib/readline.o (.bss .bss.*)
  lib/console.o (.bss .bss.*)
  lib/syscall.o (.bss .bss.*)
  user/shell.o (.bss .bss.*)
  user/main.o (.bss .bss.*)
  *(.dynbss)
  *(.bss .bss.*)
  *(COMMON)
}
PROVIDE(UBSS_end = .);
PROVIDE(end = .);

/DISCARD/ : {
  *(.eh_frame .note.GNU-stack)
}
}
