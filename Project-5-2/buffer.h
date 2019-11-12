#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#define BUFFER_SIZE 10
#define MAX_SLEEP_TIME 3

typedef int buffer_item;

int insert_item(buffer_item item);
int remove_item(buffer_item *item);
void *producer(void *param);
void *consumer(void *param);
void init_buffer();
void destroy_buffer();

#endif
