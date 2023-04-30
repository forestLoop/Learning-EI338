/**
 * Driver.c
 *
 * Schedule is in the format
 *
 *  [name] [priority] [CPU burst]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"

#define SIZE    100

int main(int argc, char *argv[]) {
    FILE *in;
    // Each FILE object denotes a C stream.
    char *temp;
    char task[SIZE];
    char *name;
    int priority;
    int burst;
    in = fopen(argv[1], "r");
    // std::FILE* fopen( const char* filename, const char* mode );
    // filename	-	file name to associate the file stream to
    // mode	-	null-terminated character string determining file access mode
    // "r"	read
    // If successful, returns a pointer to the object that controls 
    // the opened file stream, with both eof and error bits cleared.
    while (fgets(task, SIZE, in) != NULL) {
        // char *fgets( char *restrict str, int count, FILE *restrict stream );
        // str	-	pointer to an element of a char array
        // count	-	maximum number of characters to write
        // stream	-	file stream to read the data from
        // retuen value : str on success, null pointer on failure.
        temp = strdup(task);
        // char *strdup( const char *src );
        // Returns a pointer to a null-terminated byte string, 
        // which is a duplicate of the string pointed to by src. 
        // The returned pointer must be passed to free to avoid a memory leak.
        name = strsep(&temp, ",");
        // char *strsep(char **restrict stringp, const char *restrict delim);
        priority = atoi(strsep(&temp, ","));
        burst = atoi(strsep(&temp, ","));
        // add the task to the scheduler's list of tasks
        add(name, priority, burst);
        free(temp);
    }
    fclose(in);
    // invoke the scheduler
    schedule();
    return 0;
}
