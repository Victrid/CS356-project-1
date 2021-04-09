#ifndef __PTREE__
#define __PTREE__

#include <sys/types.h>

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

#endif /* __PTREE__ */
