#include <stdio.h>
#include <unistd.h>
#include "pool.h"

void *test_func(void *);

int main() {
    pool_t the_pool;
    int i=3, j=6, k=7;

    printf("starting thread pool\n");
    new_thread_pool(&the_pool, 8);
    printf("thread pool started\n");

    printf("starting thread runs\n");
    start_task(&the_pool, test_func, &i);
    start_task(&the_pool, test_func, &j);
    start_task(&the_pool, test_func, &k);
    printf("started thread runs\n");

    sleep(1);

    printf("cleaning up thread pool\n");
    destroy_pool(&the_pool);/* */
    printf("cleaned up thread pool\n");
    return 0;
}

void *test_func(
        void *vnum) {
    int num = * (int *) vnum;
    int i;
    printf("test_func started\n");

    for (i=0; i<num; i++) {
        printf("num=%d i=%d\n", num, i);
    }

    printf("test_func exitting\n");
    return NULL;
}


