/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "threadpool.h"

struct data {
    int a;
    int b;
};

void add(void *param) {
    struct data *temp;
    temp = (struct data *)param;
    printf("I add two values %d and %d result = %d\n", temp->a, temp->b, temp->a + temp->b);
}

int main(void) {
    // create some work to do
    int n = 0;
    while(n <= 0) {
        printf("Please input the number of tasks: ");
        scanf("%d", &n);
    }
    // initialize the thread pool
    pool_init();
    // submit the work to the queue
    struct data *params = malloc(n * sizeof(struct data));
    for(int i = 0; i != n; ++i) {
        params[i].a = i;
        params[i].b = -i;
        while(pool_submit(&add, &params[i]) != 0) {
            // printf("Oh no, the queue is full, sleep for a while.\n");
            // sleep(1);
            ;   // busy waiting is enough
        }
    }
    printf("All tasks have been submitted, wait for a while to exit.\n");
    sleep(3);
    pool_shutdown();
    free(params);
    return 0;
}
