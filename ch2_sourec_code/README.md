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
