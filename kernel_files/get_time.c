#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>

asmlinkage void sys_get_time(unsigned long* start_sec,unsigned long* start_nsec)
{
    struct timespec now;
    get_runningstimeofday(&now);
    *start_sec = now.tv_sec;
    *start_nsec = now.tv_nsec;
    return;
}

