#include <linux/jiffies.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 128
#define PROC_NAME "jiffies"

/**
 * Function prototypes
 */
static ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

// #define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
// LINUX_VERSION_CODE return the Linux version 
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0) // #include <linux/version.h>
#define HAVE_PROC_OPS
#endif

#ifdef HAVE_PROC_OPS
static struct proc_ops proc_ops = {
        .proc_read = proc_read
        /*
        static const struct proc_ops proc_statsdelta_ops = {
	    .proc_read	= proc_read,
	    .proc_open	= proc_statsdelta_open,
	    .proc_release	= proc_close,
	    .proc_lseek	= default_llseek,
        };

        
        struct proc_ops {
        	unsigned int proc_flags;
        	int	(*proc_open)(struct inode *, struct file *);
        	ssize_t	(*proc_read)(struct file *, char __user *, size_t, loff_t *);
        	ssize_t (*proc_read_iter)(struct kiocb *, struct iov_iter *);
        	ssize_t	(*proc_write)(struct file *, const char __user *, size_t, loff_t *);
        	loff_t	(*proc_lseek)(struct file *, loff_t, int);
        	int	(*proc_release)(struct inode *, struct file *);
        	__poll_t (*proc_poll)(struct file *, struct poll_table_struct *);
        	long	(*proc_ioctl)(struct file *, unsigned int, unsigned long);
        #ifdef CONFIG_COMPAT
        	long	(*proc_compat_ioctl)(struct file *, unsigned int, unsigned long);
        #endif
        	int	(*proc_mmap)(struct file *, struct vm_area_struct *);
        	unsigned long (*proc_get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
        } __randomize_layout;
        */
};
#else
static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
};
#endif

/* This function is called when the module is loaded. */
int proc_init(void)
{

        // creates the /proc/hello entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &proc_ops);

        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {

        // removes the /proc/hello entry
        remove_proc_entry(PROC_NAME, NULL);

        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/hello is read.
 * 
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 *
 * params:
 *
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */
static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;

        rv = sprintf(buffer, "Current value if jifies: %lu\n", jiffies);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);
        /*
        unsigned long   copy_to_user (	void __user * to, const void * from, unsigned long n);
        */

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("MIT");
MODULE_DESCRIPTION("Jiffies Module");
MODULE_AUTHOR("qqwqqw689");
