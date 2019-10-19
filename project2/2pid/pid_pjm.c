/**
 * Kernel module that communicates with /proc file system.
 *
 * This provides the base logic for Project 2 - displaying task information
 */

#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>

#define BUFFER_SIZE 128
#define PROC_NAME "pid"

/* the current pid */
static long l_pid;
static long pid_state;
static char command[BUFFER_SIZE];

/**
 * Function prototypes
 */
static ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
        .write = proc_write,
};

/* This function is called when the module is loaded. */
static int proc_init(void)
{
    // creates the /proc/procfs entry
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}

/* This function is called when the module is removed. */
static void proc_exit(void) 
{
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
static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    struct task_struct *tsk = NULL;

    if (completed) {
            completed = 0;
            return 0;
    }
    printk("ready to check");
    printk("PID-> %ld\n", l_pid);
    /*tsk = pid_task(find_vpid(l_pid), PIDTYPE_PID);
    if (tsk){
    	printk("null pid task found");
    	return -1;
    }*/
    completed = 1;
    printk("ready to transform\n");
    printk("pid-> %ld\n", l_pid);
    printk("%s", command);
    printk("%ld", pid_state);
    rv = sprintf(buffer, "command=[%s] pid=[%ld] state=[%ld]\n", command, l_pid, pid_state);
    if (copy_to_user(usr_buf, buffer, rv)) {
        printk( KERN_INFO "Error copying from user\n");
        rv = -1;
    }
    return rv;
}


static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos)
{
    char *k_mem;
    char mem[100];
    k_mem = kmalloc(count, GFP_KERNEL);
    if (copy_from_user(k_mem, usr_buf, count)) {
        printk( KERN_INFO "Error copying from user\n");
        return -1;
    }

    printk(KERN_INFO "K_MEM-> %s\n", k_mem);
    sscanf(k_mem,"%ld\n%s", &l_pid, mem);

    struct task_struct *tsk = NULL;
    if (pid_task(find_vpid(l_pid), PIDTYPE_PID)){
    	tsk = pid_task(find_vpid(l_pid), PIDTYPE_PID);
	strcpy(command, tsk->comm);
	pid_state = tsk->state;
    }else{
    	printk("no correct pid found");
    }

    kfree(k_mem);
    return count;
}


module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module");
MODULE_AUTHOR("CMQ");
