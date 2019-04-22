#define _GNU_SOURCE
#include "task.h"
#include "util.h"
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>
#include <unistd.h>
#define GET_TIME 314
#define PRINTK 315

int set_task_to_core(int pid, int core){
	if (core > sizeof(cpu_set_t)) {
		fprintf(stderr, "Core index out of range.");
		return -1;
	}

	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core, &mask);
		
	if (sched_setaffinity(pid, sizeof(mask), &mask) < 0) {
		perror("sched_setaffinity");
		exit(1);
	}

	return 0;
}

int task_build(struct task t){
	int pid = fork();

	if (pid < 0) {
		perror("fork");
		return -1;
	}

	if (pid == 0) {
		unsigned long start_sec, start_nsec, end_sec, end_nsec;
		char dmesg_log[200];

		// get start time
		start_sec = get_current_second();
		start_nsec = get_current_nsecond();

		for (int i = 0; i < t.t_running; i++) {
			run_unit_time();
		}

		// get end time
		end_sec = get_current_second();
		end_nsec = get_current_nsecond();

		sprintf(dmesg_log, "[project1] %d %lu.%09lu %lu.%09lu\n", getpid(), start_sec, start_nsec, end_sec, end_nsec);
		// printf("%s\n", dmesg_log);
		syscall(PRINTK, dmesg_log);
		exit(0);
	}
	
	// Assign child to another core prevent from interupted by parant
	set_task_to_core(pid, CORE_C);

	return pid;
}

int set_priority(int pid, int policy){
	struct sched_param param;
	
	// SCHED_IDLE should set priority to 0
	param.sched_priority = 0;

	int ret = sched_setscheduler(pid, policy, &param);
	
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}

	return ret;
}

int set_low_priority(int pid){
	return set_priority(pid, SCHED_IDLE);
}

int set_high_priority(int pid){
	return set_priority(pid, SCHED_OTHER);
}
