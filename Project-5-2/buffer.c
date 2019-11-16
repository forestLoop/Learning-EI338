#include <stdlib.h> // rand()
#include <stdio.h>
#include <unistd.h> // sleep()
#include <semaphore.h>
#include <pthread.h>
#include "buffer.h"

// the buffer
buffer_item buffer[BUFFER_SIZE + 1];
size_t bufferHead, bufferTail;

// mutex and semaphore
sem_t full, empty;
pthread_mutex_t lock;

// insert item into buffer
// return 0 if successful, otherwise nonzero
int insert_item(buffer_item item) {
    sem_wait(&empty);
    pthread_mutex_lock(&lock);
    buffer[bufferTail] = item;
    bufferTail = (bufferTail + 1) % (BUFFER_SIZE + 1);
    pthread_mutex_unlock(&lock);
    sem_post(&full);
    return 0;
}

// remove an object from buffer and place it in item
// return 0 if successful, otherwise nonzero
int remove_item(buffer_item *item) {
    sem_wait(&full);
    pthread_mutex_lock(&lock);
    *item = buffer[bufferHead];
    bufferHead = (bufferHead + 1) % (BUFFER_SIZE + 1);
    pthread_mutex_unlock(&lock);
    sem_post(&empty);
    return 0;
}

void *producer(void *param) {
    buffer_item item;
    while (1) {
        // sleep for a random period of time
        sleep(rand() % MAX_SLEEP_TIME + 1);
        item = rand();
        if (insert_item(item)) {
            fprintf(stderr, "Error: failed to insert item.\n");
        } else {
            printf("Producer %i produced %d.\n", *(int *)param, item);
        }
    }
}

void *consumer(void *param) {
    buffer_item item;
    while (1) {
        // sleep for a random period of time
        sleep(rand() % MAX_SLEEP_TIME + 1);
        if (remove_item(&item)) {
            fprintf(stderr, "Error: failed to remove item.\n");
        } else {
            printf("Consumer %i consumed %d.\n", *(int *)param, item);
        }
    }
}

void init_buffer() {
    pthread_mutex_init(&lock, NULL);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);
    bufferHead = bufferTail = 0;
}

void destroy_buffer() {
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&lock);
}
