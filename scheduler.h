#ifndef _SCHEDULING_H_
#define _SCHEDULING_H_

#include "task.h"

#define FIFO    1
#define RR      2
#define SJF     3
#define PSJF    4

// Running scheduler
int scheduling(struct task *task, int task_all, int policy);

#endif
