# Project 2-1: UNIX Shell

UNIX Shell. (Operating System Concepts, 10th Edition, Chapter 2)

## Description

This project consists of designing a C program to serve as a shell interface that accepts user commands and then executes each command in a separate process. Your implementation will support input and output redirection, as well as pipes as a form of IPC between a pair of commands. Completing this project will involve using the UNIX `fork()` , `exec()` ,` wait()` , `dup2()` , and `pipe()` system calls and can be completed on any Linux, UNIX , or mac OS system.

## Environment

- OS: Ubuntu 18.04 (Linux kernel version: 5.3.5)
- Compiler: GCC 7.4.0 

## Basic Ideas

To implement a simple shell, basically, I need to read input from the user, parse the input, and execute the command accordingly. Besides, for I/O redirection, I need to read and write files and carefully bind `stdin` and `stdout` to files. For simplicity, this project only requires a single pipe, rather than multiple chained pipes, which is much harder to implement. So roughly, I just `fork()` some sub-processes and communicate between them with `pipe()`.

The `main()` function of my program looks like this:

```c
int main(void) {
    char *args[MAX_LINE / 2 + 1]; /* command line (of 80) has max of 40 arguments */
    char command[MAX_LINE + 1];
    init_args(args);
    init_command(command);
    while (1) {
        printf("osh>");
        fflush(stdout);
        fflush(stdin);
        /* Make args empty before parsing */
        refresh_args(args);
        /* Get input and parse it */
        if(!get_input(command)) {
            continue;
        }
        size_t args_num = parse_input(args, command);
        /* Continue or exit */
        if(args_num == 0) { // empty input
            printf("Please enter the command! (or type \"exit\" to exit)\n");
            continue;
        }
        if(strcmp(args[0], "exit") == 0) {
            break;
        }
        /* Run command */
        run_command(args, args_num);
    }
    refresh_args(args);     // to avoid memory leaks!
    return 0;
}
```



## Details

Here let's focus on some implementation details in the project. 

**By the way, you may directly refer to the source code, which is well commented enough.**

### Input and Parse (History)

This function reads input from `stdin` and also handles `!!` (last command in history).

```c
int get_input(char *command) {
    char input_buffer[MAX_LINE + 1];
    if(fgets(input_buffer, MAX_LINE + 1, stdin) == NULL) {
        fprintf(stderr, "Failed to read input!\n");
        return 0;
    }
    if(strncmp(input_buffer, "!!", 2) == 0) {
        if(strlen(command) == 0) {  // no history yet
            fprintf(stderr, "No history available yet!\n");
            return 0;
        }
        printf("%s", command);    // keep the command unchanged and print it
        return 1;
    }
    strcpy(command, input_buffer);  // update the command
    return 1;
}
```

This function parses the input and splits it into several tokens. The key is the usage of `strtok()`.

```c
size_t parse_input(char *args[], char *original_command) {
    size_t num = 0;
    char command[MAX_LINE + 1];
    strcpy(command, original_command);  // make a copy since `strtok` will modify it
    char *token = strtok(command, DELIMITERS);
    while(token != NULL) {
        args[num] = malloc(strlen(token) + 1);
        strcpy(args[num], token);
        ++num;
        token = strtok(NULL, DELIMITERS);
    }
    return num;
}
```

### Concurrency

When there's an ampersand ('&') at the end of input, the shell needs to execute the command concurrently, i.e., in the background. To implement this, first check the existence of the ampersand:

```c
int run_concurrently = check_ampersand(args, &args_num);
```

Then in the parent process:

```c
if(!run_concurrently) { // parent and child run concurrently
    wait(NULL);
}
```

### I/O Redirection

After parsing the input, we need to check whether there're '<' and '>' in the command to determine the I/O redirection. If so, some file will be opened and bound to `stdin` or `stdout` with `dup2()`.

First, we have a function to check whether to redirect I/O (some error handling code is omitted here). It looks through arguments and returns a flag (bit 1 for output and bit 0 for input).

```c
unsigned check_redirection(char **args, size_t *size, char **input_file, char **output_file) {
    unsigned flag = 0;
    size_t to_remove[4], remove_cnt = 0;
    for(size_t i = 0; i != *size; ++i) {
        if(strcmp("<", args[i]) == 0) {     // input
            to_remove[remove_cnt++] = i;
            flag |= 1;
            *input_file = args[i + 1];
            to_remove[remove_cnt++] = ++i;
        } else if(strcmp(">", args[i]) == 0) {   // output
            to_remove[remove_cnt++] = i;
            flag |= 2;
            *output_file = args[i + 1];
            to_remove[remove_cnt++] = ++i;
        }
    }
    /* Remove I/O indicators and filenames from arguments */
    for(int i = remove_cnt - 1; i >= 0; --i) {
        size_t pos = to_remove[i];  // the index of arg to remove
        while(pos != *size) {
            args[pos] = args[pos + 1];
            ++pos;
        }
        --(*size);
    }
    return flag;
}
```

Then, with `io_flag` and file names, do the redirection (error handling code is omitted here):

```c
int redirect_io(unsigned io_flag, char *input_file, char *output_file, int *input_desc, int *output_desc) {
    if(io_flag & 2) {  // redirecting output
        *output_desc = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 644);
        dup2(*output_desc, STDOUT_FILENO);
    }
    if(io_flag & 1) { // redirecting input
        *input_desc = open(input_file, O_RDONLY, 0644);
        dup2(*input_desc, STDIN_FILENO);
    }
    return 1;
}
```

After execution, never forget to close these opened files!

```c
void close_file(unsigned io_flag, int input_desc, int output_desc) {
    if(io_flag & 2) {
        close(output_desc);
    }
    if(io_flag & 1) {
        close(input_desc);
    }
}
```

### Pipe

Similar to I/O redirection, when handling pipe, first check the pipe operator '|' and split all augments into two parts: one for the first command and the other for the second command.

```c
void detect_pipe(char **args, size_t *args_num, char ***args2, size_t *args_num2) {
    for(size_t i = 0; i != *args_num; ++i) {
        if (strcmp(args[i], "|") == 0) {
            free(args[i]);
            args[i] = NULL;
            *args_num2 = *args_num -  i - 1;
            *args_num = i;
            *args2 = args + i + 1;
            break;
        }
    }
}
```

Then in the execution,  use `fork()` to create one more process and establish a `pipe()` between them to communicate. (also, code for error handling and I/O redirection is omitted here)

```c
if(args_num2 != 0) {    // pipe
    /* Create pipe */
    int fd[2];
    pipe(fd);
    /* Fork into another two processes */
    pid_t pid2 = fork();
    if(pid2 == 0) {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        execvp(args2[0], args2);
        close(fd[0]);
    } else if(pid2 > 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        execvp(args[0], args);
        close(fd[1]);
}
```

## Result

Here's some tests for its functionalities:

```bash
osh>ls -a
.  ..  Makefile  README.md  simple_shell  simple_shell.c  simple_shell.o

osh>!!
ls -a
.  ..  Makefile  README.md  simple_shell  simple_shell.c  simple_shell.o

osh>ls > test_io.txt

osh>sort < test_io.txt
Makefile
README.md
simple_shell
simple_shell.c
simple_shell.o
test_io.txt

osh>ls -al | sort
drwxrwxrwx 1 root root  4096 Nov  1 22:57 .
drwxrwxrwx 1 root root  4096 Oct 29 21:33 ..
-rwxrwxrwx 1 root root 11497 Nov  1 22:56 simple_shell.c
-rwxrwxrwx 1 root root   158 Oct 22 19:53 Makefile
-rwxrwxrwx 1 root root 17888 Nov  1 22:56 simple_shell
-rwxrwxrwx 1 root root    74 Nov  1 22:57 test_io.txt
-rwxrwxrwx 1 root root  8236 Oct 29 22:56 README.md
-rwxrwxrwx 1 root root  9048 Nov  1 22:56 simple_shell.o
total 56

osh>cat < test_io.txt | sort > test_io_sorted.txt

osh>cat test_io_sorted.txt
Makefile
README.md
simple_shell
simple_shell.c
simple_shell.o
test_io.txt

osh>
Please enter the command! (or type "exit" to exit)

osh>exit
```

Or the corresponding screenshot:

![Screenshot](./screenshot.png)

