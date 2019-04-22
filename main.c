#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include "task.h"
#include "util.h"
#include "scheduler.h"

int get_policy_from_str(char* str_policy);

int main(int argc, char* argv[]){
	// uncomment this line if you want to measure time unit on your own machine
	// measure_time_unit();

	char str_policy[256];
	int task_all;

	scanf("%s", str_policy);
	scanf("%d", &task_all);
	
	struct task *tasks = (struct task *)malloc(task_all * sizeof(struct task));

	for (int i = 0; i < task_all; i++) {
		scanf("%s%d%d", tasks[i].name,
			&tasks[i].t_ready, &tasks[i].t_running);
	}

	int policy = get_policy_from_str(str_policy);
	scheduling(tasks, task_all, policy);

	exit(0);
}

int get_policy_from_str(char* str_policy){
	if (strcmp(str_policy, "FIFO") == 0) {
		return FIFO;
	}
	if (strcmp(str_policy, "RR") == 0) {
		return RR;
	}
	if (strcmp(str_policy, "SJF") == 0) {
		return SJF;
	}
	if (strcmp(str_policy, "PSJF") == 0) {
		return PSJF;
	}
	fprintf(stderr, "Invalid policy: %s", str_policy);
	exit(1);
}