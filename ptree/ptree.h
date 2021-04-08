#ifndef __PTREE__
#define __PTREE__

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/unistd.h>

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
#endif /* __PTREE__ */