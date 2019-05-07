#ifndef TASK_H
#define TASK_H

#include <inc/trap.h>
#include <kernel/mem.h>
#include <kernel/spinlock.h>
#define NR_TASKS	64
#define TIME_QUANT	100

typedef enum
{
	TASK_FREE = 0,
	TASK_RUNNABLE,
	TASK_RUNNING,
	TASK_SLEEP,
	TASK_STOP,
} TaskState;

// Each task's user space
#define USR_STACK_SIZE	(40960)

typedef struct Task
{
	int task_id;
	int parent_id;
	struct Trapframe tf; //Saved registers
	int32_t remind_ticks;
	TaskState state;	//Task state
  pde_t *pgdir;  //Per process Page Directory
	
} Task;

// Lab6
// 
// Design your Runqueue structure for cpu
// your runqueue sould have at least two
// variables:
//
// 1. an index for Round-robin scheduling
//
// 2. a list indicate the tasks in the runqueue
//
typedef struct Runqueue
{
    int cur_task;
    Task *tasks[NR_TASKS];
    int task_num;
    struct spinlock lock;
} Runqueue;

void runqueue_add_task(struct Runqueue *rq, struct Task *task);
void runqueue_remove_task(struct Runqueue *rq, int pid);

void task_init();
void task_init_percpu();
void env_pop_tf(struct Trapframe *tf);

/*
 * Interface for real implementation of kill and fork
 * Since their real implementation should be in kernel/task.c
 */
void sys_kill(int pid);
int sys_fork();

#endif
