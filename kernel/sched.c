#include <kernel/task.h>
#include <inc/x86.h>

#define ctx_switch(ts) \
  do { env_pop_tf(&((ts)->tf)); } while(0)

/*
* Implement a simple round-robin scheduler (Start with the next one)
*
* 1. You have to remember the task you picked last time.
*
* 2. If the next task is in TASK_RUNNABLE state, choose
*    it.
*
* 3. After your choice set cur_task to the picked task
*    and set its state, remind_ticks, and change page
*    directory to its pgdir.
*
* 4. CONTEXT SWITCH, leverage the macro ctx_switch(ts)
*    Please make sure you understand the mechanism.
*/
void sched_yield(void)
{
	extern Task tasks[];
	extern Task *cur_task;

	int cur_pid = cur_task->task_id;
	do {
	    cur_pid = (cur_pid + 1) % NR_TASKS;
    } while (tasks[cur_pid].state != TASK_RUNNABLE);

    cur_task = &tasks[cur_pid];
    cur_task->state = TASK_RUNNING;
    lcr3(PADDR(cur_task->pgdir));
    ctx_switch(cur_task);
}
