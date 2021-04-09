#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>    /* kmalloc */
#include <linux/uaccess.h> /* kernel-user memory */
#include <linux/unistd.h>

#include "ptree.h"
#include "kassert.h"

MODULE_LICENSE("GPL");

/**
 * genprinfo  -   generate prinfo from task_struct
 * @ts: task_struct pointer
 * @pr: destination prinfo location
 */
void genprinfo(ts_t* ts, prinfo_t* pr) {
    /* kassert is a macro defined in kassert.h */
    kassert(ts != NULL && pr != NULL, return 0);
    pr->parent_pid       = ts->parent == NULL ? 0 : ts->parent;
    pr->pid              = ts->pid;
    /* get_first_pid is a macro defined in ptree.h */
    pr->first_child_pid  = get_first_pid(&(ts->children));
    pr->next_sibling_pid = get_first_pid(&(ts->sibling));
    pr->state            = ts->state;
    pr->uid              = ts->cred->uid;
    get_task_comm(pr->comm, ts);
    return;
}

/**
 * dfs  -   Deep-First search the task_struct, use static to prevent running
 * from other file.
 * @ts:  root task_struct
 * @ptr: pointer of pointer to the avavilable empty space for prinfo. This is
 *       to simulate the *& job in C++, while reference is not available for
 *       C.
 * @end: When *ptr == end, the function will not stop writing but continue to
 *       traverse the process tree.
 * @return: returning the sons count under the root pointer.
 */
static int dfs(ts_t* ts, prinfo_t** ptr, prinfo_t* end) {
    int count;
    ts_t* iter;
    kassert(ts != NULL && *ptr != NULL, return 0);
    count = 1;
    if (*ptr < end) {
        genprinfo(ts, *ptr);
        (*ptr)++;
    }
    list_for_each_entry(iter, &(ts->children), sibling) 
        count += dfs(iter, ptr, end);
    return count;
}

/**
 * ptree    -   the ptree function required.
 * @buf:  userspace buffer pointer
 * @nr:   userspace buffer length pointer
 * @return: total process count when correct.
 *          will return -1 if error happened.
 */
int ptree(struct prinfo* buf, int* nr) {
    int real_nr, count;
    prinfo_t *itend, *itbegin, **iter, *pend;
    
    /** Check for boundary conditions: NULL pointers, fail to get user 
     *  data, or the nr given by user is less than 1.
     * 
     *  ktry is a macro defined in kassert.h
     */
    ktry(buf == NULL || nr == NULL, FAIL);
    ktry(get_user(real_nr, nr), FAIL);
    ktry(real_nr<=0, FAIL);

    /** itbegin and itend points to the buffer's head and tail, the pend
     *  points to the end bound of the buffer, and iter points to the tail
     *  which would be used in dfs.
     */
    itbegin = (prinfo_t*)kmalloc(real_nr * sizeof(prinfo_t), GFP_KERNEL);
    itend   = itbegin;
    iter    = &itend;
    pend    = itbegin + real_nr;
    ktry(itbegin == NULL, FAIL);

    read_lock(&tasklist_lock);
    count = dfs(&init_task, iter, pend);
    read_unlock(&tasklist_lock);

    /* When copy_to_user failed we must free the allocated memories. */
    ktry(copy_to_user(buf, itbegin, (itend - itbegin)* sizeof(prinfo_t)),
        FAIL_FREE);
    kfree(itbegin);
    ktry(put_user(itend - itbegin ,nr), FAIL);
    return count;

    FAIL_FREE:
    kfree(itbegin);
    FAIL:
    return -1;
}

/* Syscall part, this is identical to the given examples. */

#define __NR_PTREECALL 356
long* const syscall = (long*)0xc000d8c4;
unsigned long oldcall;
static int addsyscall_init(void) {
    oldcall                 = syscall[__NR_PTREECALL];
    syscall[__NR_PTREECALL] = (unsigned long)ptree;
    printk(KERN_INFO "ptree module loaded!\n");
    return 0;
}

static void addsyscall_exit(void) {
    syscall[__NR_PTREECALL] = oldcall;
    printk(KERN_INFO "ptree module removed!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);
