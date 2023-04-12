# Project 2-1: UNIX Shell

UNIX Shell. (Operating System Concepts, 10th Edition, Chapter 3)

## Description

This project consists of designing a C program to serve as a shell interface that accepts user commands and then executes each command in a separate process. Your implementation will support input and output redirection, as well as pipes as a form of IPC between a pair of commands. Completing this project will involve using the UNIX `fork()` , `exec()` ,` wait()` , `dup2()` , and `pipe()` system calls and can be completed on any Linux, UNIX , or mac OS system.

## Environment

- OS: Ubuntu 22.04 
- Compiler: GCC 11.3.0 

## Basic Ideas

To implement a simple shell, basically, I need to read input from the user, parse the input, and execute the command accordingly. Besides, for I/O redirection, I need to read and write files and carefully bind `stdin` and `stdout` to files. For simplicity, this project only requires a single pipe, rather than multiple chained pipes, which is much harder to implement. So roughly, I just `fork()` some sub-processes and communicate between them with `pipe()`.


## Result

the corresponding screenshot:

![Screenshot](./screenshot.png)

