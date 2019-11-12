/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be
// completed by a thread in the pool
typedef struct {
    void (*function)(void *p);
    void *data;
}
task;

// mutex and semaphore
pthread_mutex_t lock;   // mutex lock for enqueue and dequeue
sem_t taskCnt;

// the work queue
task taskQueue[QUEUE_SIZE + 1];     // one extra entry needed for determining whether the queue is full
size_t queueHead = 0, queueTail = 0;

// the worker bees
pthread_t bees[NUMBER_OF_THREADS];

// insert a task into the queue
// returns 0 if successful or 1 otherwise,
int enqueue(task t) {
    pthread_mutex_lock(&lock); // acquire lock before modifying the task queue
    if((queueTail + 1) % (QUEUE_SIZE + 1) == queueHead) {  // the queue is full
        pthread_mutex_unlock(&lock);
        return 1;
    }
    taskQueue[queueTail] = t;
    queueTail = (queueTail + 1) % (QUEUE_SIZE + 1);
    pthread_mutex_unlock(&lock);
    return 0;
}

// remove a task from the queue
task dequeue() {
    pthread_mutex_lock(&lock); // acquire lock before modifying the task queue
    task ret = taskQueue[queueHead];
    queueHead = (queueHead + 1) % (QUEUE_SIZE + 1);
    pthread_mutex_unlock(&lock);    // remember to release the lock
    return ret;
}

// the worker thread in the thread pool
void *worker(void *param) {
    // execute the task
    task workToDo;
    while(TRUE) {
        sem_wait(&taskCnt); // block until there is an available task, also as a cancellation point
        workToDo = dequeue();
        execute(workToDo.function, workToDo.data);
    }
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p) {
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p) {
    int err = 0;
    task newTask;
    newTask.function = somefunction;
    newTask.data = p;
    err = enqueue(newTask);
    if(!err) {   // success
        sem_post(&taskCnt);     // signal the semaphore
    }
    return err;
}

// initialize the thread pool
void pool_init(void) {
    pthread_mutex_init(&lock, NULL);
    sem_init(&taskCnt, 0, 0);
    for(size_t i = 0; i != NUMBER_OF_THREADS; ++i) {
        pthread_create(&bees[i], NULL, worker, NULL);
    }
}

// shutdown the thread pool
void pool_shutdown(void) {
    for(size_t i = 0; i != NUMBER_OF_THREADS; ++i) {
        pthread_cancel(bees[i]);
        pthread_join(bees[i], NULL);
    }
    sem_destroy(&taskCnt);
    pthread_mutex_destroy(&lock);
}
