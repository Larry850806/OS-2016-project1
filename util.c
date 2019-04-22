#include <time.h>
#include <stdio.h>
#include "util.h"

void run_unit_time() {
	volatile unsigned long i;
	for (i = 0; i < 1000000UL; i++);
}

unsigned long get_current_second(){
	struct timespec tps;
	if (clock_gettime(CLOCK_REALTIME, &tps) != 0) {
		perror("clock_gettime");
		return -1;
	}
	return tps.tv_sec;
}

unsigned long get_current_nsecond(){
	struct timespec tp;
	if (clock_gettime(CLOCK_REALTIME, &tp) != 0) {
		perror("clock_gettime");
		return -1;
	}
	return tp.tv_nsec;
}

void measure_time_unit(){
	unsigned long start_sec, start_nsec, end_sec, end_nsec;
	char log[200];

	// get start time
	start_sec = get_current_second();
	start_nsec = get_current_nsecond();

	for (int i = 0; i < 1000; i++) {
		run_unit_time();
	}

	// get end time
	end_sec = get_current_second();
	end_nsec = get_current_nsecond();

	sprintf(log, "[time measuring] run 1000 units from %lu.%09lu to %lu.%09lu\n", start_sec, start_nsec, end_sec, end_nsec);
	printf("%s\n", log);
}