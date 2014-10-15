/*
 * pool.h
 *
 * Set of functions to implement a simple thread pool.
 * Functionality exposed:
 *  1. Create new thread pool with number of threads specified
 *  2. Start a particular task within pool
 *  3. Check if task is complete
 *  4. Clean up thread pool on exit
 *
 * Author: Prasanna Rao
 */

#ifndef __POOL_H
#define __POOL_H
#include <pthread.h>

/* Structure to define a task */
typedef struct {
    void *(*run)(void *);
    void *args;
    pthread_cond_t start;
    pthread_mutex_t smutex;
    int die;
    int done;
} task_t;

/* Structure to hold tasks and threads */
typedef struct {
    pthread_t *threads;
    task_t *tasks;
    unsigned int num_tasks;
    unsigned int next;
} pool_t;

/* Create new thread with specified number of threads */
extern void new_thread_pool(pool_t *self, unsigned int num_tasks);

/* start specified task */
extern int start_task(pool_t *self, void *(*run_func)(void *), void *args);

/* check if task is complete - no wait */
extern int task_complete(pool_t *self, int task_id);

/* cleanup pool on exit */
extern void destroy_pool(pool_t *self);
#endif
