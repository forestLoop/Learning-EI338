#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "buffer.h"

int main(int argc, char *argv[]) {
    /* 1. Get command line arguments argv[1],argv[2],argv[3] */
    if(argc != 4) {
        fprintf(stderr, "Usage: <executable> sleepSecond numProducer numConsumer\n" );
        return 0;
    }
    unsigned int sleepSecond = atoi(argv[1]);
    size_t numProducer = atoi(argv[2]), numConsumer = atoi(argv[3]);
    /* 2. Initialize buffer */
    init_buffer();
    /* 3. Create producer thread(s) */
    pthread_t *producers = malloc(numProducer * sizeof(pthread_t));
    int *producer_id = malloc(numProducer * sizeof(pthread_t));
    for(size_t i = 0 ; i != numProducer; ++i) {
        producer_id[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &producer_id[i]);
    }
    /* 4. Create consumer thread(s) */
    pthread_t *consumers = malloc(numConsumer * sizeof(pthread_t));
    int *consumer_id = malloc(numConsumer * sizeof(pthread_t));
    for(size_t i = 0; i != numConsumer; ++i) {
        consumer_id[i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &consumer_id[i]);
    }
    /* 5. Sleep */
    printf("Sleep for %u second(s) before exit.\n", sleepSecond);
    sleep(sleepSecond);
    /* 6. Exit */
    for(size_t i = 0; i != numProducer; ++i) {
        pthread_cancel(producers[i]);
        pthread_join(producers[i], NULL);
    }
    sleep(3);   // a little time for consumers to consume left items
    for(size_t i = 0; i != numConsumer; ++i) {
        pthread_cancel(consumers[i]);
        pthread_join(consumers[i], NULL);
    }
    free(producers);
    free(producer_id);
    free(consumers);
    free(consumer_id);
    destroy_buffer();
    return 0;
}
