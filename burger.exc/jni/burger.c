/**
 *      burger   -   Solve the burger buddies problem.
 *
 *  Solve the burger buddies problem. 
 *  Usage: /path/to/burger racksize cooks cashiers customers
 *
 */

#include <errno.h>
#include <pthread.h> /* Threading */
#include <stdio.h>
#include <stdlib.h>    /* malloc */
#include <unistd.h>

#include "burger.h"

/* Universal semaphores */
sem_t customer;      /* has customer */
sem_t customer_list; /* customer list mutex */
sem_t canteen_open;  /* mutex locked when canteen is open */

sem_t burger_rack_empty; /* initially set to 0 */
sem_t burger_rack_full;  /* initially set to racksize */


/* Information to init different roles */
struct staff_info {
    int number;
};

struct customer_info {
    int number;
    sem_t burger; /* the order had been placed */
    sem_t served; /* customer had been entertained  */
    struct customer_info* next;
};

/** This waitlist is global for all cashiers
 *  Just for selection, does not mean customers to be served sequentially
*/
struct customer_info* waitlist = NULL;

/**
 * @brief The instance of cashier
 * @param vconfigs
 * @return (void)
 */
void* ins_cashier(void* vconfigs) {
    struct customer_info* curr_customer;
    struct staff_info* info;
    info = (struct staff_info*)vconfigs;
    printf("-A%d\tcashier %d on duty.\n", info->number, info->number);
    while (1) {
        staff_timed_wait_quit(customer, canteen_open, cashier, A, info);
        /* cashier entertain customer */
        sem_wait(&customer_list);
        curr_customer = waitlist;
        waitlist      = waitlist->next;
        sem_post(&customer_list);
        printf("-A%d\tcashier %d accept customer %d order.\n", info->number,
               info->number, curr_customer->number);

        /* cashier obtain burger, serve and next */
        staff_timed_wait_quit(burger_rack_empty, canteen_open, cashier, A,
                              info);
        sem_post(&burger_rack_full);
        printf("-A%d\tcashier %d serving a burger to customer %d.\n",
               info->number, info->number, curr_customer->number);
        sem_post(&(curr_customer->burger));
        staff_timed_wait_quit(curr_customer->served, canteen_open, cashier, A,
                              info);
        printf("-A%d\tcashier %d entertained customer %d and ready for next.\n",
               info->number, info->number, curr_customer->number);
    }
    return NULL;
}

/**
 * @brief The instance of customer
 * @param vconfigs configs
 * @return (void)
 */
void* ins_customer(void* vconfigs) {
    struct customer_info *info, *waitptr;
    info = (struct customer_info*)vconfigs;
    printf("-U%d\tcustomer %d comes.\n", info->number, info->number);
    /* customer initialization */
    if (sem_init(&(info->burger), 0, 0) || sem_init(&(info->served), 0, 0)) {
        printf("!U%d\tcustomer %d cannot place order and left.\n", info->number,
               info->number);
        return NULL;
    }
    /* enlist customer */
    sem_wait(&customer_list);
    if (waitlist == NULL) {
        waitlist = info;
    } else {
        waitptr = waitlist;
        while (waitptr->next != NULL)
            waitptr = waitptr->next;
        waitptr->next = info;
    }
    sem_post(&customer_list);
    sem_post(&customer);

    /* wait to get it's burger */
    customer_timed_wait_quit(info->burger, canteen_open, info);
    printf("-U%d\tcustomer %d received burger and leave.\n", info->number,
           info->number);
    /* leave */
    sem_post(&(info->served));
    return NULL;
}

/**
 * @brief The instance of cook
 * @param vconfigs configs
 * @return (void)
 */
void* ins_cook(void* vconfigs) {
    struct staff_info* info;
    info = (struct staff_info*)vconfigs;
    printf("-O%d\tcook %d on duty.\n", info->number, info->number);
    while (1) {
        staff_timed_wait_quit(burger_rack_full, canteen_open, cook, O, info);
        printf("-O%d\tcook %d made a burger.\n", info->number, info->number);
        sem_post(&burger_rack_empty);
    }
    return NULL;
}

int main(int argc, char** argv) {
    int racksize, cooks, cashiers, customers;
    int i; /* iterator */
    pthread_t *cooks_list, *cashiers_list, *customers_list;
    /* Argument check and load */
    if (argc == 5) {
        /* atoi is no-throw guaranteed */
        racksize  = atoi(argv[1]);
        cooks     = atoi(argv[2]);
        cashiers  = atoi(argv[3]);
        customers = atoi(argv[4]);
    } else {
        printf("Invalid argument.\n");
        printf("Usage: %s racksize cooks cashiers customers\n", argv[0]);
        return -1;
    }
    if (racksize <= 0 || cooks <= 0 || cashiers <= 0 || customers < 0) {
        printf("Invalid argument.\n");
        printf("racksize cooks cashiers should be positive, and"
               " customers should be no less than zero\n");
        return -1;
    }

    /* init semaphores and exit when error happend */
    init_semaphore(customer, 0);
    init_semaphore(customer_list, 1);
    init_semaphore(canteen_open, 0);
    init_semaphore(burger_rack_empty, 0);
    init_semaphore(burger_rack_full, racksize);
    printf("Start\n");
    printf("-------------------\n");
    
    /* Open the canteen */
    role_spawn(cook, cooks, staff_info, cooks_list, ins_cook, );
    role_spawn(cashier, cashiers, staff_info, cashiers_list, ins_cashier, );
    role_spawn(customer, customers, customer_info, customers_list, ins_customer,
               newone->next = NULL);

    /* Wait all customers served and close the canteen */
    join_list(customers_list, customers);
    sem_post(&canteen_open);
    join_list(cashiers_list, cashiers);
    join_list(cooks_list, cooks);
    /* The required parent process message */
    printf("-------------------\n");
    printf("Complete.\n");
    return 0;
}
