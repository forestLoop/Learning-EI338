# This is a source code for  Introduction to Linux Kernel Modules (chapter 2)


## details about Makefile

### Makefile

'''

	obj-m += simple.o
	all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
	clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
  
'''

#### explanations

obj-y += foo.o

This tell kbuild that there is one object in that directory named foo.c. foo.o will be build from foo.c or foo.S.

all target denotes that if you invoke it, make will build all what's needed to make a complete build. 

The purpose of the make utility is to determine automatically which pieces of a large program 
need to be recompiled, and issue the commands to recompile them.

-C dir option: Change to directory dir before reading the makefiles or doing anything else.

/lib/modules: This directory contains directories with kernel modules needed for the kernel.

Uname is a command-line tool in Linux-based operating systems that prints or displays information about the kernel, system architecture, operating system, hardware, version information, and more.( -r This option prints the kernel release.)


shell: it takes as an argument a shell command and expands to the output of the command. 

The M= option causes that makefile to move back into your module source directory before trying to build the modules target.
With the M variable, the makefile knows where your actual project files are and can change back to that location.

 PWD: a internal variable or function of Makefile(need some help???)
 
 clean: This is an optional rule. It allows you to type 'make clean' at the command line to get rid of your object and executable files.
 
 ## Procedures
 '''
 
	make
	
	sudo insmod hello.ko
	
	sudo dmesg
	
	vat /proc/hello
	
	sudo rmmod hello
	
	sudo dmesg
 '''
