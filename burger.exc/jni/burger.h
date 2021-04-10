/**
 * sched.h  -   timed_wait function, error handling and initiation macros
 * 
 */

#ifndef _BURGER_
#define _BURGER_

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h> /* Error handling */

/* Wait timeout interval */
#define __time_interval__ 1

/**
 * @brief provides a wait method which can check an additional mutex lock for 
 * release.
 * @param sem the waiting mutex
 * @param mut the canteen_open mutex
 * @return int 0 if released, -1 for others.
 * @retval When mutex lock is released, return -1 and set errno to EHOSTDOWN. 
 * when the wait finished return 0, and keep the errno if other situations 
 * occured.
 * 
 * In POSIX standard, semaphore waiting is handled by scheduler and should not
 * cause system-time consuming, even if no other processes can be sheduled to
 * run.
 *
 * In POSIX standard 1.c, errno is thread-specific and should not be affected 
 * by other threads.
 */
int timed_wait(sem_t* sem, sem_t* mut) {
    struct timespec spec;
WAIT:
    clock_gettime(CLOCK_REALTIME, &spec);
    spec.tv_sec += __time_interval__;
    if (sem_timedwait(sem, &spec)) {
        if (errno != ETIMEDOUT)
            /* Pass on the issue */
            return -1;
        /* reset the errno */
        errno = 0;
        if (!sem_trywait(mut)) {
            /* mut released */
            /* relocked the mut, must unlock */
            sem_post(mut);
            errno = EHOSTDOWN;
            return -1;
        } else {
            /* mut kept */
            if (errno != EAGAIN)
                /* Pass on the issue */
                return -1;
            errno = 0;
            /* continue to wait */
            goto WAIT;
        }
    } else {
        if (!sem_trywait(mut)) {
            sem_post(mut);
            errno = EHOSTDOWN;
            return -1;
        } else {
            if (errno != EAGAIN)
                return -1;
            errno = 0;
            return 0;
        }
    }
}

/**
 * @brief staff wait semaphore, if mut released then quit.
 * @param sem the waiting mutex
 * @param mut the canteen_open mutex
 * @param role role name
 * @param r role abbreviation
 * @param info the role's info pointer
 * @return NULL if error occured
 */
#define staff_timed_wait_quit(sem, mut, role, r, info)                         \
    if (timed_wait(&sem, &mut)) {                                              \
        /* wait failed */                                                      \
        if (errno == EHOSTDOWN) {                                              \
            /* canteen closed */                                               \
            printf("-" #r "%d\t" #role "%d off duty.\n", info->number,         \
                   info->number);                                              \
            return NULL;                                                       \
        } else {                                                               \
            printf("-" #r "%d\t" #role "%d\tleft with error: %s\n",            \
                   info->number, info->number, strerror(errno));               \
            return NULL;                                                       \
        }                                                                      \
    }

/**
 * @brief customer wait semaphore, if mut released then quit.
 * @param sem the waiting mutex
 * @param mut the canteen_open mutex
 * @param role role name
 * @param r role abbreviation
 * @param info the role's info pointer
 * @return NULL if error occured
 */
#define customer_timed_wait_quit(sem, mut, info)                               \
    if (timed_wait(&sem, &mut)) {                                              \
        /* wait failed */                                                      \
        if (errno == EHOSTDOWN) {                                              \
            /* canteen closed */                                               \
            printf("-U%d\tcustomer%d\tleft because canteen closed.\n",         \
                   info->number, info->number);                                \
            return NULL;                                                       \
        } else {                                                               \
            printf("!U%d\tcustomer%d\tleft with error: %s\n", info->number,    \
                   info->number, strerror(errno));                             \
            return NULL;                                                       \
        }                                                                      \
    }

/**
 * @brief init semaphore, quit if error happend.
 * @param semahphore_name semaphore name
 * @param init_value set the init value
 * @return nothing, or return -1 early if error happend.
 */
#define init_semaphore(semaphore_name, init_value)                             \
    if (sem_init(&semaphore_name, 0, init_value)) {                            \
        printf("Initing " #semaphore_name " failed.\nError:%s\n",              \
               strerror(errno));                                               \
        return -1;                                                             \
    }

/**
 * @brief assertion in creation
 * @param assertion if assertion is true then quit early.
 * @return nothing, or return -1 early if error happend.
 */
#define assert_creation(assertion)                                             \
    if (assertion) {                                                           \
        printf("Creation failed.\nError:%s\n", strerror(errno));               \
        return -1;                                                             \
    }

/**
 * @brief spawn a role
 * @param role role name
 * @param roles role name (plural)
 * @param info the role's info pointer
 * @param list the thread list name for all spawned threads
 * @param instance the instance function for pthread to create
 * @param additional additional code fore spawning
 * @return nothing, or assert when error happened as above.
 */
#define role_spawn(role, roles, info, list, instance, additional)              \
    list = (pthread_t*)malloc(roles * sizeof(pthread_t));                      \
    assert_creation(list == NULL);                                             \
    for (i = 0; i < roles; i++) {                                              \
        struct info* newone = (struct info*)malloc(sizeof(struct info));       \
        assert_creation(newone == NULL);                                       \
        newone->number = i;                                                    \
        additional;                                                            \
        assert_creation(                                                       \
            pthread_create(list + i, NULL, instance, (void*)newone));          \
    }

/**
 * @brief join pthreads in the list
 * @param listname list name
 * @param listsize list size
 * @return nothing
 */
#define join_list(listname, listsize)                                          \
    for (i = 0; i < listsize; i++)                                             \
        pthread_join(listname[i], NULL);

#endif /* _BURGER_ */