#include <kernel/task.h>
#include <kernel/timer.h>
#include <kernel/mem.h>
#include <kernel/cpu.h>
#include <kernel/spinlock.h>
#include <kernel/syscall.h>
#include <kernel/trap.h>
#include <inc/stdio.h>

static struct spinlock puts_lock;

void do_puts(char *str, uint32_t len)
{
    spin_lock(&puts_lock);

	uint32_t i;
	for (i = 0; i < len; i++)
	{
		k_putch(str[i]);
	}

    spin_unlock(&puts_lock);
}

int32_t do_getc()
{
	return k_getc();
}

int32_t do_syscall(uint32_t syscallno, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
	int32_t retVal = -1;
    Task *cur_task = thiscpu->cpu_task;

	switch (syscallno)
	{
	case SYS_fork:
		/*
     * You can reference kernel/task.c, kernel/task.h
     */
        retVal = sys_fork();
		break;

	case SYS_getc:
		retVal = do_getc();
		break;

	case SYS_puts:
		do_puts((char*)a1, a2);
		retVal = 0;
		break;

	case SYS_getpid:
		/*
     * Get current task's pid
     */
        retVal = cur_task->task_id;
		break;

	case SYS_getcid:
		/* Lab6: get current cpu's cid */
		retVal = thiscpu->cpu_id;
		break;

	case SYS_sleep:
		/*
     * Yield this task
     * You can reference kernel/sched.c for yielding the task
     */
        cur_task->state = TASK_SLEEP;
        cur_task->remind_ticks = a1;
        extern void sched_yield();
        sched_yield();
		break;

	case SYS_kill:
		/*
     * Kill specific task
     * You can reference kernel/task.c, kernel/task.h
     */
        sys_kill(a1);
		break;

  case SYS_get_num_free_page:
		/*
     * You can reference kernel/mem.c
     */
    retVal = sys_get_num_free_page();
    break;

  case SYS_get_num_used_page:
		/*
     * You can reference kernel/mem.c
     */
    retVal = sys_get_num_used_page();
    break;

  case SYS_get_ticks:
		/*
     * You can reference kernel/timer.c
     */
    retVal = sys_get_ticks();
    break;

  case SYS_settextcolor:
		/*
     * You can reference kernel/screen.c
     */
    sys_settextcolor(a1, a2);
    break;

  case SYS_cls:
		/*
     * You can reference kernel/screen.c
     */
    sys_cls();
    break;

	}
	return retVal;
}

static void syscall_handler(struct Trapframe *tf)
{
	/*
   * call do_syscall
   * Please remember to fill in the return value
   * HINT: You have to know where to put the return value
   */
    int ret = do_syscall(tf->tf_regs.reg_eax, tf->tf_regs.reg_edx,
                         tf->tf_regs.reg_ecx, tf->tf_regs.reg_ebx,
                         tf->tf_regs.reg_edi, tf->tf_regs.reg_esi);
    tf->tf_regs.reg_eax = ret;

}

void syscall_init()
{
  /*
   * Please set gate of system call into IDT
   * You can leverage the API register_handler in kernel/trap.c
   */
    extern void SYSCALL();
    register_handler(T_SYSCALL, syscall_handler, SYSCALL, 1, 3);
    spin_initlock(&puts_lock);

}

