/**
 *      ptreetest   -   Test ptree.
 *
 *  Test the ptree by given ptree location.
 *  Usage: /path/to/ptreetest /path/to/ptree
 *
 */

#include <errno.h>
#include <string.h> /* Error handling */
#include <stdio.h>
#include <sys/types.h> /* pid_t */
#include <stdlib.h> /* malloc */
#include <unistd.h>


#define _student_id_ "519021911045"

int main(int argc, char** argv) {
    /* Invalid argument */
    if(argc < 2){
        printf("Invalid argument\n");
        printf("Usage: %s <path to ptree>\n",argv[0]);
        return -1;
    }

    pid_t pid;
    pid = fork();
    /* fork error */
    if (pid < 0) {
        printf("fork failed: %s\n", strerror(errno));
        return -1;
    }

    if (pid) {
        /* Parent */
        pid = getpid();
        printf(_student_id_ "Parent is %d\n", pid);
        wait(NULL);
        /* required output message */
        printf("Execution complete. Exiting..\n");
        return 0;
    } else {
        /* child */
        pid = getpid();
        printf(_student_id_ "Child is %d\n", pid);
        /* path and arg[0] */
        if (execl(argv[1], argv[1], NULL)) {
            printf("execl error: %s\n", strerror(errno));
        }
        /* This should not be reached when execl succeeds */
        return -1;
    }
    return 0;
}
