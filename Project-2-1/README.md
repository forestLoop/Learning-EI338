# Project 2-1: UNIX Shell

UNIX Shell. (Operating System Concepts, 10th Edition, Chapter 3)

## Description

This project consists of designing a C program to serve as a shell interface that accepts user commands and then executes each command in a separate process. Your implementation will support input and output redirection, as well as pipes as a form of IPC between a pair of commands. Completing this project will involve using the UNIX `fork()` , `exec()` ,` wait()` , `dup2()` , and `pipe()` system calls and can be completed on any Linux, UNIX , or mac OS system.

## Environment

- OS: Ubuntu 18.04 (Linux kernel version: 5.3.5)
- Compiler: GCC 7.4.0 

## Basic Ideas

To implement a simple shell, basically, I need to read input from the user, parse the input, and execute the command accordingly. Besides, for I/O redirection, I need to read and write files and carefully bind `stdin` and `stdout` to files. For simplicity, this project only requires a single pipe, rather than multiple chained pipes, which is much harder to implement. So roughly, I just `fork()` some sub-processes and communicate between them with `pipe()`.


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

