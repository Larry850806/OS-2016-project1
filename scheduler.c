#define _GNU_SOURCE
#include "task.h"
#include "scheduler.h"
#include "util.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>

/*
	- time_last: the last context switch time (for RR scheduling)
  - time_now : current unit time
  - running  : current running task, initiate with -1
  - task_done: number of process finished
  - rr_status: the last rr status
*/ 

static int time_last;
static int time_now;
static int running;
static int task_done;
static int rr_status = 0;

// different policy for task schedling 
int fifo_task(struct task *tasks, int task_all);
int sjf_task(struct task *tasks, int task_all);
int psjf_task(struct task *tasks, int task_all);
int rr_task(struct task *tasks, int task_all);

// method for sorting by ready time
int cmp(const void *t1, const void *t2) {
	return ((struct task *)t1)->t_ready - ((struct task *)t2)->t_ready;
}

int scheduling(struct task *tasks, int task_all, int policy){
	// sort taskes by ready time
	qsort(tasks, task_all, sizeof(struct task), cmp);

	// initial pid = -1, imply not ready
	for (int i = 0; i < task_all; i++) {
		tasks[i].pid = -1;
	}

	// initial scheduler
	time_now = 0;
	running = -1;
	task_done = 0;
	
	// assign schedluer to CORE_P
	set_task_to_core(getpid(), CORE_P);
	
	// set high priority to scheduler, prevent it from being disruptted
	set_high_priority(getpid());
	
	
	while(1) {
		// Check if running task finish
		if (running != -1 && tasks[running].t_running == 0) {
			//kill(running, SIGKILL);
			waitpid(tasks[running].pid, NULL, 0); //wait ture task run finish
			printf("%s %d\n", tasks[running].name, tasks[running].pid);
			rr_status = running;
			running = -1;
			task_done++;

			// All task finish
			if (task_done == task_all)
				break;
		}

		// Check if task ready and execute
		for (int i = 0; i < task_all; i++) {
			if (tasks[i].t_ready == time_now) {
				tasks[i].pid = task_build(tasks[i]);  // fork() = 0
				set_low_priority(tasks[i].pid);
			}
		}
		int next = -1;
		// Select next running  task
		if (policy == FIFO) {
			next = fifo_task(tasks, task_all);
		} else if (policy == SJF) {
			next = sjf_task(tasks, task_all);
		} else if (policy == PSJF) {
			next = psjf_task(tasks, task_all);
		} else if (policy == RR) {
			next = rr_task(tasks, task_all);
		} else {
			fprintf(stderr, "Invalid policy: %d", policy);
		}
		
		if (next != -1) {
			// Context switch
			if (running != next) {
				set_high_priority(tasks[next].pid);
				set_low_priority(tasks[running].pid);
				running = next;
				time_last = time_now;
			}
		}

		run_unit_time();
		if (running != -1) {
			tasks[running].t_running--;
		}
			
		time_now++;
	}

	return 0;
}
// Return index of next task 
int fifo_task(struct task *tasks, int task_all){
	// Non-preemptive
	if (running != -1) {
		return running;
	}
		

	int ret = -1; // initial index of next task to -1

	for(int i = 0; i < task_all; i++) {
		// if tasks[i] not ready or already finish
		if(tasks[i].pid == -1 || tasks[i].t_running == 0) {
			continue;
		}
		// if no task running or i task ready 	
		if(ret == -1 || tasks[i].t_ready < tasks[ret].t_ready) {
			ret = i;
		}
	}

	return ret;
}

int sjf_task(struct task *tasks, int task_all){
	// non-preemptive
	if (running != -1){
		return running;
	}
		
	// initial index of next task to -1
	int ret = -1; 

	for (int i = 0; i < task_all; i++) {
		// if tasks[i] not ready or already finish
		if (tasks[i].pid == -1 || tasks[i].t_running == 0)  {
			continue;
		}
		// if no task running or tasks[i].t_running < tasks[ret].t_running
		if (ret == -1 || tasks[i].t_running < tasks[ret].t_running) {
			ret = i;
		}
	}

	return ret;
}

int psjf_task(struct task *tasks, int task_all){
	 // initial index of next task to -1
	int ret = -1;
	for (int i = 0; i < task_all; i++) {
		// if tasks[i] not ready or already finish
		if (tasks[i].pid == -1 || tasks[i].t_running == 0) {
			continue;
		}
		// if no task running or tasks[i].t_running < tasks[ret].t_running
		if (ret == -1 || tasks[i].t_running < tasks[ret].t_running) {
			ret = i;
		}
	}

	return ret;
}

int rr_task(struct task *tasks, int task_all){
	// initial index of next task to -1
	int ret = -1;

	if (running == -1) {
		for (int i = 0; i < task_all; i++) {
			int cur_i = (rr_status + i) % task_all;
			// if tasks[cur_i] ready and not finish
			if (tasks[cur_i].pid != -1 && tasks[cur_i].t_running > 0){ 
				ret = cur_i;
				rr_status = ret + 1;
				break;
			}
		}
	// arrive time slice
	} else if ((time_now - time_last) % 500 == 0)  {  
		// cycle turn in task_all task
		ret = (running + 1) % task_all;   
		// if tasks[ret] not ready or finish
		while (tasks[ret].pid == -1 || tasks[ret].t_running == 0) {
			// turn to next
			ret = (ret + 1) % task_all;  
		}	
	} else {
		ret = running;
	}
	return ret;

}
