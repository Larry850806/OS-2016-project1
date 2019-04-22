#include <linux/kernel.h>
#include <linux/linkage.h>

asmlinkage void sys_finish_process(char* str)
{
    printk("%s",str);
    return;
}
