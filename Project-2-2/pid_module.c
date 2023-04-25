#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <linux/version.h>

#define BUFFER_SIZE 128
#define PROC_NAME "pid"

/* the current pid */
static int current_pid;


static ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0) // #include <linux/version.h>
#define HAVE_PROC_OPS
#endif
// Designated initializers for aggregate types : .a = b
#ifdef HAVE_PROC_OPS
static struct proc_ops proc_ops = {
        .proc_read = proc_read,
        .proc_write = proc_write,
};
#else
static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
    .write = proc_write,
};
#endif

/* This function is called when the module is loaded. */
static int proc_init(void) {
    // creates the /proc/procfs entry
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    // struct proc_dir_entry *proc_create(const char *name, umode_t mode, struct proc_dir_entry *parent, const struct proc_ops *proc_ops);
    // name: the name of the new file to be created. 
    // mode: the permissions of the new file.
    // parent: a pointer to the parent directory of the new file. 
    // proc_ops: a pointer to a proc_ops structure that defines the operations that can be performed on the new file.
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    //  KERN_INFO is the log level 
    return 0;
}

/* This function is called when the module is removed. */
static void proc_exit(void) {
    // removes the /proc/procfs entry
    remove_proc_entry(PROC_NAME, NULL);
    printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/pid is read.
 *
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the
 * corresponding /proc file.
 */

 // ssize_t : a signed integer type
 // char __user is a type qualifier used in 
 // Linux kernel programming to indicate that a pointer argument points to user space memory. 
static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    struct task_struct *tsk = NULL;
    if (completed) {
        completed = 0;
        return 0;
    }
    tsk = pid_task(find_vpid(current_pid), PIDTYPE_PID);
    // struct pid *find_vpid(int nr)
    // find_vpid() finds the pid by its virtual id, i.e. in the current namespace
    // struct task_struct *pid_task(struct pid *pid, enum pid_type type)
    // PIDTYPE_PID: This value indicates that the PID is a regular process ID.
    /*
    enum pid_type
    {
    PIDTYPE_PID,
    PIDTYPE_PGID,
    PIDTYPE_SID,
    PIDTYPE_MAX
    };
    */
    // pid : Pointer to the struct pid of the process. 
    // the task_struct structure contains all the information about a process
    // comm : executable name of a process
    //  the state of a process
    if(tsk) {
        rv = snprintf(buffer, BUFFER_SIZE,
                      "command = [%s], pid = [%d], state = [%u]\n",
                      tsk->comm, current_pid, tsk->__state);
        // int snprintf ( char * s, size_t n, const char * format, ... );
        // s : Pointer to a buffer where the resulting C-string is stored.
        // n : Maximum number of bytes to be used in the buffer.
        // return : The number of characters that would have been written
        // unsigned int			__state;
    } else {
        printk(KERN_INFO "Invalid PID %d!", current_pid);
        return 0;
    }
    completed = 1;
    // copies the contents of kernel buffer to userspace usr_buf
    if (raw_copy_to_user(usr_buf, buffer, rv)) {
        rv = -1;
        // unsigned long raw_copy_to_user(void *to, const void *from, unsigned long n);
        // n parameter specifies the number of bytes to copy from the source buffer
    }
    return rv;
}

/* This function is called each time we write to the /proc file system. */
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos) {
    char *k_mem;
    // allocate kernel memory
    k_mem = kmalloc(count, GFP_KERNEL);
    // void * kmalloc (	size_t size, gfp_t flags);
    // GFP_KERNEL - Allocate normal kernel ram.
    
    /* copies user space usr_buf to kernel buffer */
    if (raw_copy_from_user(k_mem, usr_buf, count)) {
        // unsigned long raw_copy_from_user(void *to, const void __user *from, unsigned long n);
        printk( KERN_INFO "Error copying from user\n");
        return -1;
    }
    k_mem[count] = '\0';   // make sure k_mem is null-terminated
    kstrtoint(k_mem, 10, &current_pid);
    // int kstrtoint(const char * s, unsigned int base, int * res);
    // s : The start of the string.
    // base : The number base to use.
    // res : Where to write the result of the conversion on success.

    printk(KERN_INFO "Set current PID to %d", current_pid);
    kfree(k_mem);
    return count;
}

/* Macros for registering module entry and exit points. */
module_init( proc_init );
// The module_init() macro defines which function is to be called at module insertion time 
// (if the file is compiled as a module), or at boot time
module_exit( proc_exit );
// This macro defines the function to be called at module removal time 
// (or never, in the case of the file compiled into the kernel).

MODULE_LICENSE("GPL");  // MIT is not allowed here, because this module calls pid_task(), which is GPL only
MODULE_DESCRIPTION("Report the task information when /proc/pid is read, after PID is written to /proc/pid.");
MODULE_AUTHOR("Keith Null");


