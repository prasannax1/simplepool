/*
 * pool_impl.c
 *
 * Implementation of functions declared in pool.h.
 * Functionality exposed:
 *  1. Create new thread pool with number of threads specified
 *  2. Start a particular task within pool
 *  3. Check if task is complete
 *  4. Clean up thread pool on exit
 *
 * Additional functionality implemented:
 *  1. Default thread function
 *  2. Limited increment
 *
 * Author: Prasanna Rao
 */

#include <stdlib.h>
#include <pthread.h>
#include "pool.h"

void * default_run(
        void *arg) {
    /*
     * name: default_run
     * description: This is the default function passed to pthread_create
     * working: waits till start signal then runs specified function
     * args: task descriptor which contains signal, function, args, etc.
     * returns: NULL
     */
    task_t *t = (task_t *) arg;

    do {
        pthread_mutex_lock(&(t->smutex));
        pthread_cond_wait(&(t->start), &(t->smutex));
        
        if (!t->die) {
            t->run(t->args);
        }

        t->done = 1;
        pthread_mutex_unlock(&(t->smutex));
    } while (!t->die);
    return NULL;
}


void new_thread_pool(
        pool_t *self,
        unsigned int n) {
    /*
     * name: new_thread_pool
     * description: create a new thread pool with specified size
     * working: allocates memory and starts default threads in pool
     * args: 
     *  1. self - pointer to pool struct
     *  2. n - number of threads to create
     * returns: void
     */
    int i;
    self->threads = (pthread_t *)calloc(n, sizeof(pthread_t));
    self->tasks = (task_t *)calloc(n, sizeof(task_t));
    self->num_tasks = n;
    self->next = 0;

    for (i=0; i<n; ++i) {
        self->tasks[i].die = 0;
        self->tasks[i].done = 1;
        pthread_mutex_init(&(self->tasks[i].smutex), NULL);
        pthread_cond_init(&(self->tasks[i].start), NULL);

        pthread_create(
                &(self->threads[i]), 
                NULL, 
                default_run, 
                &(self->tasks[i]));
    }
}

int lim_incr(int i, int n) {
    /*
     * name: lim_incr
     * description: Increment to a limit then cycle back
     * args: 
     *  1. i = initial value to increment
     *  2. n = limit to which to increment
     * returns: incremented value or zero
     */
    return i == n ? 0 : i+1;
}

int start_task(
        pool_t *self,
        void *(*run)(void *),
        void *args) {
    /*
     * name: start_task
     * description: start specified task with thread from pool
     * working: checks free thread from pool, populates task 
     * descriptor with function and args and signals the thread
     * to run
     *
     * args: 
     *  1. self - pointer to pool struct
     *  2. run - function to run
     *  3. args - arguments to pass to run function
     * returns: index of started task in task pool or -1 if none free
     */
    unsigned int i = self->next;
    do {
        if (self->tasks[i].done) {
            pthread_mutex_lock(&(self->tasks[i].smutex));
            self->tasks[i].args = args;
            self->tasks[i].run = run;
            self->tasks[i].done = 0;
            pthread_cond_signal(&(self->tasks[i].start));
            pthread_mutex_unlock(&(self->tasks[i].smutex));

            self->next = lim_incr(self->next, self->num_tasks);
            return i;
        } else {
            i = lim_incr(i, self->num_tasks);
        }
    } while (self->next != i);
    return -1;
}

void destroy_pool(
        pool_t *self) {
    /*
     * name: destroy_pool
     * description: cleanup a specified thread pool
     * working: signals all threads to stop, waits and frees memory
     * args: 
     *  1. self - pointer to pool struct
     * returns: void
     */
    int i;
    for (i=0; i<self->num_tasks; ++i) {
        pthread_mutex_lock(&(self->tasks[i].smutex));
        self->tasks[i].args = NULL;
        self->tasks[i].run = NULL;
        self->tasks[i].die = 1;
        pthread_cond_signal(&(self->tasks[i].start));
        pthread_mutex_unlock(&(self->tasks[i].smutex));
    }

    for (i=0; i<self->num_tasks; ++i) {
        pthread_join(self->threads[i], NULL);
    }

    free(self->threads);
    free(self->tasks);
}

int task_complete(
        pool_t *self,
        int i) {
    /*
     * name: task_complete
     * description: checks if a task is complete or not
     * working: checks a flag that task will set on completion
     * args: 
     *  1. self - pointer to pool struct
     *  2. i - index of thread in pool
     * returns: 1 if task complete else 0
     */
    return self->tasks[i].done;
}

