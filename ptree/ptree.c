#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/threads.h> /* Used for pid_max */

#include "ptree.h"

MODULE_LICENSE("GPL");

void genprinfo(struct task_struct* ts, prinfo_t* pr){
	if (ts == NULL || pr == NULL){
		printk(KERN_ERROR "Error pointer NULL");
		return;
	}
	pr->pid = ts->pid;
	pr->state = ts->state;
	pr->uid = ts->cred->uid;
	pr->parent_pid = ts->parent == NULL ? 0 : ts->parent->pid;

}

/* Modify this for larger memory buffer */
int pid_max = PID_MAX_DEFAULT;

int ptree(struct prinfo *buf,int *nr){
	int knr = 0;
	// prinfo_t* pinfo = kmalloc(pid_max*sizeof(prinfo_t));
	// read_lock(&tasklist_lock);
	// /*TODO*/&init_task;
	// printk(sizeof(prinfo));
	// read_unlock(&tasklist_lock);
}



/* Syscall part */

#define __NR_PTREECALL 356
long * const syscall = (long*)0xc000d8c4;
unsigned long oldcall;
static int addsyscall_init(void)
{	
	oldcall = syscall[__NR_PTREECALL];
	syscall[__NR_PTREECALL] = (unsigned long )addsyscall_init;
	printk(KERN_INFO "module loaded!\n");
	printk(KERN_INFO "%d\n", pid_max);
	return 0;
}

static void addsyscall_exit(void)
{
	syscall[__NR_PTREECALL] = oldcall;
	printk(KERN_INFO "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);
