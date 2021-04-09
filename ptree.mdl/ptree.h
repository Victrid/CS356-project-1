/**
 * ptree.h 
 * 
 * this file is for prinfo definitions and some macros to get list item.
 * 
 */
#ifndef __PTREE__
#define __PTREE__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/types.h> /* pid_t */
#include <linux/unistd.h>
#include <linux/printk.h>

/* definition for prinfo */
struct prinfo {
    pid_t parent_pid;
    pid_t pid;
    pid_t first_child_pid;
    pid_t next_sibling_pid;
    long state;
    long uid;
    char comm[64];
};

typedef struct prinfo prinfo_t;
typedef struct task_struct ts_t;

/**
 * get_first_pid    -  get first pid or zero when it does not exist.
 * @listhead:   head of the list
 */
#define get_first_pid(listhead) \
    list_empty(listhead) ? 0 : list_first_entry(listhead, ts_t, sibling)->pid;

#endif /* __PTREE__ */
