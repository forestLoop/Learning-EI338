#include<string.h>
#include<stdlib.h>

#include "schedulers.h"
#include "task.h"
#include "cpu.h"
#include "list.h"

struct node *taskList[MAX_PRIORITY + 1];
struct node *next_node;

// add a task to its priority list
void add(char *name, int priority, int burst) {
    Task *t = malloc(sizeof(Task));
    // allocate memory and then copy the name
    t->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(t->name, name);
    // priority and burst
    t->priority = priority;
    t->burst = t->remaining_burst = burst;
    // insert into task list
    insert(&taskList[priority], t);
}

/* pickNextTask: pick the next task to execute with Priority + Round Robin
 * taskList must not be empty!
 */
Task *pickNextTask(struct node *tl) {
    Task *ret = next_node->task;
    next_node = (next_node -> next) ? next_node->next : tl;
    return ret;
}

// invoke the scheduler
void schedule() {
    // from higher priority to lower priority
    for(size_t p = MAX_PRIORITY; p >= MIN_PRIORITY; --p) {
        next_node = taskList[p];
        while(taskList[p]) {
            Task *t = pickNextTask(taskList[p]);
            int slice = QUANTUM < t->remaining_burst ? QUANTUM : t->remaining_burst;
            run(t, slice);
            t->remaining_burst -= slice;
            if(!t->remaining_burst) {
                delete(&taskList[p], t);
            }
        }
    }
}
