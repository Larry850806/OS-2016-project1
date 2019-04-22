#ifndef _TASK_H_
#define _TASK_H_

#include <sys/types.h>

#define CORE_C 1
#define CORE_P 0

struct task {
	char name[32];
	int t_ready;
	int t_running;
	pid_t pid;
};

/*
   Method for Tasks (Process)
   - assign a task to specific core
   - build a task by fork
   - set low priority to some task
   - set high priority to some task
*/ 

int set_task_to_core(int pid, int core);
int task_build(struct task task);
int set_low_priority(int pid);
int set_high_priority(int pid);

#endif
