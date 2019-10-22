#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_LINE 80 /* 80 chars per line, per command */
#define DELIMITERS " \t\n\v\f\r"

/*
 * Function: init_args
 * ----------------------------
 *   Initialize args, i.e., making all of its content NULL
 *
 *   args: the array to initialize
 */
void init_args(char *args[]) {
    for(size_t i = 0; i != MAX_LINE / 2 + 1; ++i) {
        args[i] = NULL;
    }
}

/*
 * Function: refresh_args
 * ----------------------------
 *   Refresh the content of args, i.e., free old content and set to NULL
 *
 *   args: the array to refresh
 */
void refresh_args(char *args[]) {
    while(*args) {
        free(*args);  // to avoid memory leaks
        *args++ = NULL;
    }
}

/*
 * Function: parse_input
 * ----------------------------
 *   Get input from the user, parse and store it
 *
 *   args: the array to put arguments
 *
 *   returns: the number of arguments
 */
size_t parse_input(char *args[]) {
    size_t num = 0;
    char input_buffer[MAX_LINE + 1], *token;
    if(fgets(input_buffer, MAX_LINE + 1, stdin) == NULL) {
        return 0;
    }
    token = strtok(input_buffer, DELIMITERS);
    while(token != NULL) {
        args[num] = malloc(strlen(token) + 1);
        strcpy(args[num], token);
        ++num;
        token = strtok(NULL, DELIMITERS);
    }
    return num;
}

/* TODO: detect '&' and set `run_in_background` */
/* TODO: execute commands in history */
/* TODO: redirecting I/O */
/* TODO: pipe */
int main(void) {
    char *args[MAX_LINE / 2 + 1]; /* command line (of 80) has max of 40 arguments */
    int should_run = 1;
    size_t args_num;
    pid_t pid;
    init_args(args);
    while (should_run) {
        printf("osh>");
        fflush(stdout);
        fflush(stdin);
        /* Make args empty before parsing */
        refresh_args(args);
        /* Get input and parse it */
        args_num = parse_input(args);
        /* Print to debug */
        for(size_t i = 0; i != args_num; ++i) {
            printf("--%s--\n", args[i]);
        }
        fflush(stdout);
        /* Continue or exit */
        if(args_num == 0) { // empty input
            printf("Please enter the command! (or type \"exit\" to exit)\n");
            continue;
        }
        if(strcmp(args[0], "exit") == 0) {
            should_run = 0;
            continue;
        }
        /* Create a child process and execute the command */
        pid = fork();
        if(pid < 0) {   // fork failed
            fprintf(stderr, "Failed to fork!\n");
            return 1;
        } else if (pid == 0) { // child process
            execvp(args[0], args);
        } else { // parent process
            wait(NULL);
        }
    }
    return 0;
}
