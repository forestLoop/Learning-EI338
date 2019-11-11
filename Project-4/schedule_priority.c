#include<string.h>
#include<stdlib.h>

#include "schedulers.h"
#include "task.h"
#include "cpu.h"
#include "list.h"

struct node *taskList = NULL;

// add a task to the list
void add(char *name, int priority, int burst) {
    Task *t = malloc(sizeof(Task));
    // allocate memory and then copy the name
    t->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(t->name, name);
    // priority and burst
    t->priority = priority;
    t->burst = burst;
    // insert into task list
    insert(&taskList, t);
}

/* pickNextTask: pick the next task to execute with the Highest Priority
 * taskList must not be empty!
 */
Task *pickNextTask() {
    Task *highest_priority_job = taskList->task;
    struct node *n = taskList;
    while(n){
        if(n->task->priority >= highest_priority_job->priority){
            highest_priority_job = n->task;
        }
        n = n->next;
    }
    return highest_priority_job;
}

// invoke the scheduler
void schedule() {
    while(taskList) {
        Task *t = pickNextTask();
        run(t, t->burst);
        delete(&taskList, t);
    }
}
