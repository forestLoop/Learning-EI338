#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LENGTH_LIMIT 128
#define OPERATOR_LENGTH_LIMIT 8

typedef struct MemBlock {
    size_t lo, hi;
    char *name;
    struct MemBlock *prev, *next;
} MemBlock;

size_t mem_size = 0;
MemBlock *mem;


MemBlock *make_block(size_t lo, size_t hi, const char *name, MemBlock *prev, MemBlock *next) {
    MemBlock *ret = malloc(sizeof(MemBlock));
    if(ret == NULL) {
        printf("Failed to allocate phisical memory.\n");
        exit(-1);
    }
    ret->lo = lo, ret->hi = hi;
    // allocate memory and copy the string
    if(strlen(name) != 0) {
        ret->name = malloc(sizeof(char) * (strlen(name) + 1));
        strcpy(ret->name, name);
    } else { // unused block
        ret->name = NULL;
    }
    // handle the prev and next to preserve a doubly-linked list
    ret->prev = prev, ret->next = next;
    if(prev) {
        prev->next = ret;
    }
    if(next) {
        next->prev = ret;
    }
    return ret;
}

int request_memory(const char *name, size_t size, char strategy) {
    MemBlock *hole = NULL;
    // select the hole
    switch(strategy) {
        case 'F': {
            hole = mem;
            while(hole) {
                if(hole->name == NULL && (hole->hi - hole->lo + 1) >= size) {
                    break;
                }
                hole = hole->next;
            }
            break;
        }
        case 'B': {
            MemBlock *cursor = mem;
            size_t min_size = -1;   // get the max number in size_t
            while(cursor) {
                size_t hole_size = (cursor-> hi - cursor->lo + 1);
                if(cursor->name == NULL && size <= hole_size && hole_size < min_size) {
                    min_size = hole_size;
                    hole = cursor;
                }
                cursor = cursor->next;
            }
            break;
        }
        case 'W': {
            MemBlock *cursor = mem;
            size_t max_size = size - 1;
            while(cursor) {
                size_t hole_size = (cursor-> hi - cursor->lo + 1);
                if(cursor->name == NULL && hole_size > max_size) {
                    max_size = hole_size;
                    hole = cursor;
                }
                cursor = cursor->next;
            }
            break;
        }
        default: {
            printf("Unknown strategy: %c\n", strategy);
            return -1;
        }
    }
    if(!hole || hole->name != NULL) {
        printf("No available memory to allocate.\n");
        return -2;
    }
    hole->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(hole->name, name);
    if(hole->hi - hole->lo + 1 == size) {   // the hole size is exactly equal to the requested size
        return 0;
    }
    hole->next = make_block(hole->lo + size, hole->hi, "", hole, hole->next);
    hole->hi = hole->lo + size - 1;
    return 0;
}

// release all blocks with the given name, and do the merges if possible
int release_memory(const char *name) {
    MemBlock *cursor = mem;
    int flag = 1;
    while(cursor) {
        if(cursor->name && strcmp(cursor->name, name) == 0) {
            free(cursor->name);
            cursor->name = NULL;    // mark it unused
            flag = 0;
        }
        // merge with the prev block if possible
        if(cursor->name == NULL && cursor->prev && cursor->prev->name == NULL) {
            MemBlock *temp = cursor->prev;
            cursor->prev = temp->prev;
            if(temp->prev) {
                temp->prev->next = cursor;
            }
            cursor->lo = temp->lo;
            free(temp);
        }
        // update the first block in memory if necessary
        if(cursor->prev == NULL) {
            mem = cursor;
        }
        cursor = cursor->next;
    }
    if(flag) {
        printf("No memory gets released!\n");
    }
    return flag;
}

void compact_memory() {
    MemBlock *cursor = mem;
    while(cursor) {
        // unused --> used, swap these two blocks
        if(cursor->name && cursor->prev && !cursor->prev->name) {
            MemBlock *prev = cursor->prev;
            prev->hi = prev->lo + (cursor->hi - cursor->lo);
            cursor->lo = prev->hi + 1;
            prev->name = cursor->name;
            cursor->name = NULL;
        }
        // unused --> unused, merge thees two blocks
        if(!cursor->name && cursor->prev && !cursor->prev->name) {
            MemBlock *prev = cursor->prev;
            cursor->lo = prev->lo;
            cursor->prev = prev->prev;
            if(cursor->prev) {
                cursor->prev->next = cursor;
            }
            free(prev);
        }
        cursor = cursor->next;
    }
}

void request_wrapper() {
    char name[NAME_LENGTH_LIMIT], strategy;
    size_t size;
    scanf("%s %zu %c", name, &size, &strategy); // unsafe but convenient
    printf(request_memory(name, size, strategy) ? "FAILURE\n" : "SUCCESS\n");
}

void release_wrapper() {
    char name[NAME_LENGTH_LIMIT];
    scanf("%s", name); // unsafe but convenient
    printf(release_memory(name) ? "FAILURE\n" : "SUCCESS\n");
}

void display_usage() {
    printf("=============================================================\n");
    printf("<this program> <memory size (in bytes)>\n");
    printf("Operations:\n");
    printf("    RQ <process name> <memory size (in bytes)> <strategy>\n"
           "        Request for a contigous block of memory (available strategies are F, W and B)\n"
           "    RL <process name>\n"
           "        Release the process's contigous block of memory\n"
           "    C\n"
           "        Compact unused holes of memory into one single block\n"
           "    STAT\n"
           "        Report the regions of free and allocated memory\n"
           "    X\n"
           "        Exit\n"
          );
    printf("=============================================================\n");
}

void display_memory() {
    printf("=============================================================\n");
    MemBlock *cursor = mem;
    while(cursor) {
        printf("[%06zu - %06zu] ", cursor->lo, cursor->hi);
        if(cursor->name) {
            printf("Process %s\n", cursor->name);
        } else {
            printf("Unused\n");
        }
        cursor = cursor->next;
    }
    printf("=============================================================\n");
}

int init(int argc, char **argv) {
    if(argc != 2) {
        printf("Incorrect number of arguments.\n");
        return -1;
    }
    sscanf(argv[1], "%zu", &mem_size);
    mem = make_block(0, mem_size - 1, "", NULL, NULL);
    printf("The size of memory is initialized to %zu bytes\n", mem_size);
    return 0;
}

void clean_up() {
    MemBlock *temp = mem;
    while(mem) {
        free(mem -> name);
        temp = mem;
        mem = mem -> next;
        free(temp);
    }
}

int main(int argc, char **argv) {
    if(init(argc, argv) != 0) {
        display_usage();
        return 0;
    }
    char op[OPERATOR_LENGTH_LIMIT];
    while(1) {
        printf("allocator> ");
        scanf("%s", op);    // unsafe but convenient
        if(strcmp(op, "RQ") == 0) {
            request_wrapper();
        } else if(strcmp(op, "RL") == 0) {
            release_wrapper();
        } else if(strcmp(op, "C") == 0) {
            compact_memory();
        } else if(strcmp(op, "STAT") == 0) {
            display_memory();
        } else if(strcmp(op, "X") == 0) {
            clean_up();
            break;
        } else {
            printf("Unrecognized operation.\n");
            display_usage();
        }
        // display_memory();
    }
    return 0;
}
