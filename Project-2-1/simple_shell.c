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
    char input_buffer[MAX_LINE + 1];
    if(fgets(input_buffer, MAX_LINE + 1, stdin) == NULL) {
        return 0;
    }
    size_t num = 0;
    char *token = strtok(input_buffer, DELIMITERS);
    while(token != NULL) {
        args[num] = malloc(strlen(token) + 1);
        strcpy(args[num], token);
        ++num;
        token = strtok(NULL, DELIMITERS);
    }
    return num;
}

/*
 * Function: check_ampersand
 * ----------------------------
 *   Check whether an ampersand (&) is in the end of args. If so, remove it
 *   from args and possibly reduce the size of args.
 *
 *   args: the array to check
 *   size: the pointer to array size
 *
 *   returns: whether an ampersand is in the end
 */
int check_ampersand(char **args, size_t *size) {
    size_t len = strlen(args[*size - 1]);
    if(args[*size - 1][len - 1] != '&') {
        return 0;
    }
    if(len == 1) {  // remove this argument if it only contains '&'
        free(args[*size - 1]);
        args[*size - 1] = NULL;
        --(*size);  // reduce its size
    } else {
        args[*size - 1][len - 1] = '\0';
    }
    return 1;
}

/* TODO: execute commands in history */
/* TODO: redirecting I/O */
/* TODO: pipe */
int main(void) {
    char *args[MAX_LINE / 2 + 1]; /* command line (of 80) has max of 40 arguments */
    init_args(args);
    while (1) {
        printf("osh>");
        fflush(stdout);
        fflush(stdin);
        /* Make args empty before parsing */
        refresh_args(args);
        /* Get input and parse it */
        size_t args_num = parse_input(args);
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
            break;
        }
        /* Detect '&' to determine whether to run concurrently */
        int run_concurrently = check_ampersand(args, &args_num);
        /* Create a child process and execute the command */
        pid_t pid = fork();
        if(pid < 0) {   // fork failed
            fprintf(stderr, "Failed to fork!\n");
            return 1;
        } else if (pid == 0) { // child process
            execvp(args[0], args);
        } else { // parent process
            if(!run_concurrently) { // parent and child run concurrently
                wait(NULL);
            }
        }
    }
    refresh_args(args);     // to avoid memory leaks!
    return 0;
}
