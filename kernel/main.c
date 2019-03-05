#include <inc/stdio.h>
#include <inc/kbd.h>
#include <inc/shell.h>
#include <inc/timer.h>
#include <inc/x86.h>
#include <kernel/trap.h>
#include <kernel/picirq.h>

void bss_init() {
    extern volatile char _BSS_START_[], bootstack[], bootstacktop[], _DATA_END_[];
    for (volatile char *ptr = _BSS_START_; ptr != bootstack; ptr++) {
        *ptr = 0;
    }
    for (volatile char *ptr = bootstacktop; ptr != _DATA_END_; ptr++) {
        *ptr = 0;
    }
}

extern void init_video(void);
void kernel_main(void)
{
    bss_init();
    init_video();

    pic_init();
    /*       You should uncomment them
    */
    kbd_init();
    timer_init();
    trap_init();

    /* Enable interrupt */
    __asm __volatile("sti");

    shell();
}
