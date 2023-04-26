#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUM 100
/* global variables are shared by threads */
int array[MAX_NUM];
size_t array_size = 0;

typedef struct {
    size_t low;
    size_t high;
} parameters;

int cmp(const void  *a, const void *b) {
    return *((int *)a) - *((int *)b);
}

/* The thread will do the sorting */
void *runner(void *param) {
    size_t low, high;
    low = ((parameters *)param)->low;
    high = ((parameters *)param)->high;
    // printf("Low: %lu, high: %lu\n", low, high);
    qsort(array + low, high - low, sizeof(int), cmp);
    // void qsort (void* base, size_t num, size_t size, int (*compar)(const void*,const void*));
    // Sorts the num elements of the array pointed to by base, each element size bytes long, 
    // using the compar function to determine the order.
    pthread_exit(0);
    // noreturn void pthread_exit(void *retval);
    // terminates the calling thread
}

void init_array() {
    printf("Please enter the number of elements:");
    scanf("%ld", &array_size);
    // %ld long int
    for(size_t i = 0; i != array_size; ++i) {
        scanf("%d", &array[i]);
    }
}

void merge_array(int *result) {
    size_t low1 = 0, high1 = array_size / 2;
    size_t low2 = array_size / 2, high2 = array_size;
    size_t i = 0;
    while(low1 < high1 && low2 < high2) {
        if(array[low1] < array[low2]) {
            result[i++] = array[low1++];
        } else {
            result[i++] = array[low2++];
        }
    }
    if(low2 < high2) {
        low1 = low2, high1 = high2;
    }
    while(low1 < high1) {
        result[i++] = array[low1++];
    }
}

void print_array(int *arr, size_t size) {
    for(size_t i = 0; i != size; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    pthread_t tid[2];
    pthread_attr_t attr; /* set of attributes for the thread */
    init_array();
    printf("Original array:\n");
    print_array(array, array_size);
    parameters data[2];
    data[0].low = 0, data[0].high = array_size / 2;
    data[1].low = array_size / 2, data[1].high = array_size;
    /* get the default attributes */
    pthread_attr_init(&attr);
    // int pthread_attr_init(pthread_attr_t *attr);
    // initializes the thread attributes object pointed to by attr with default attribute values.
    for(size_t i = 0; i != 2; ++i) {
        pthread_create(&tid[i], &attr, runner, &data[i]);
        // int pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr, void *(*start_routine)(void *), void *restrict arg);
    }
    /* now wait for the thread to exit */
    for(size_t i = 0; i != 2; ++i) {
        pthread_join(tid[i], NULL);
        // int pthread_join(pthread_t thread, void **retval);
        //waits for the thread specified by thread to terminate.
    }
    printf("Thread 0:\n");
    print_array(array, array_size / 2);
    printf("Thread 1:\n");
    print_array(array + array_size / 2, array_size - array_size / 2);
    int *sorted_array = malloc(sizeof(int) * array_size);
    merge_array(sorted_array);
    printf("After merging:\n");
    print_array(sorted_array, array_size);
    free(sorted_array)
    return 0;
}

