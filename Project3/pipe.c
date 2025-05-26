#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#include "buff_helper.h"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("SC2022/2023");
MODULE_DESCRIPTION("Simple pipe character device driver");
MODULE_VERSION("0:0.1");

/** module functions **/
static int pipe_init(void);
static void pipe_exit(void);

/** device driver functions  **/
static int pipe_open(struct inode *inode, struct file *filp);
static int pipe_release(struct inode *inode, struct file *filp);
ssize_t pipe_read(struct file *filep, char __user *buff, size_t count, loff_t *offp);
ssize_t pipe_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp);

/**  global variables for the device driver  **/
static dev_t mydev;
static struct cdev *mycdev;
static struct file_operations myfops = {
    .owner = THIS_MODULE,
	.read = pipe_read,
	.write = pipe_write,
	.open = pipe_open,
	.release = pipe_release
};
/***********/
#define BUFFER_MAX_SIZE 100

char data_space[BUFFER_MAX_SIZE];
circ_buff_t pipe_buffer = { 
    data_space,
    BUFFER_MAX_SIZE,
    0,
    0
};
/************/
static int pipe_open(struct inode *inode, struct file *filp)
{
	filp->private_data = inode->i_cdev;

	printk(KERN_INFO "Private data linked\n");

	nonseekable_open(inode, filp);

	// Debug:
	printk(KERN_INFO "pipe_open(): Returning\n");
    return 0;
}
/************/
static int pipe_release(struct inode *inode, struct file *filp)
{
	// Debug:
    printk(KERN_INFO "pipe_release(): Returning\n");
    return 0;
}
/************/
ssize_t pipe_read(struct file *filep, char __user *buff, size_t count, loff_t *offp)
{
    // Should read data from circ_buffer and transfer it to user space.
	char kbuffer[64];
	int try;

	int j;
	for (j = 0; j < count; j++)
	{
		try = circ_buff_pop(&pipe_buffer, &kbuffer[j]);
		if (try == -1) break;
	}
	kbuffer[j] = '\0';

	copy_to_user(buff, kbuffer, j);
	
	// Debug:
	printk(KERN_INFO "pipe_read(): Read %d characters\n", j);
	return j;
}
/************/
ssize_t pipe_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp)
{
	char kbuffer[64];

	copy_from_user(kbuffer, buff, count);
	kbuffer[count] = '\0';

	printk(KERN_INFO "message: %s", kbuffer);

    // Should write data from user space to circ_buffer.
	int i;
	for (i = 0; i < count; i++)
	{
		circ_buff_push(&pipe_buffer, kbuffer[i]);
	}

	// DON'T FORGET TO NEVER RETURN A CONSTANT ZERO VALUE

	// Debug:
	printk(KERN_INFO "pipe_write(): Wrote %d characters\n", count-1);
	return count? count:-1;
}
/**************/
static int pipe_init(void)
{
	int return_of_alloc_chrdev_region = -1;
	int return_of_cdev_add = -1;

	// Register device driver so that:
	// - Driver name is pipe 
	// - Major number is dynamically assigned
	// - Minor number starts from 0
	// - Only one device needs to be managed by the driver

	return_of_alloc_chrdev_region = alloc_chrdev_region(&mydev, 0, 1, "pipe");

	if (return_of_alloc_chrdev_region < 0){
	 	printk(KERN_ERR "Failed to register pipe device driver\n");
	 	return return_of_alloc_chrdev_region;
	}

    // Create struture to represent char devices
	mycdev = cdev_alloc();
	mycdev->owner = THIS_MODULE;
	mycdev->ops = &myfops;

    // Register character device into the kernel
	return_of_cdev_add = cdev_add(mycdev, mydev, 1);

    if (return_of_cdev_add < 0){
         printk(KERN_ERR "Failed to register character device info\n");
         return return_of_cdev_add;
    }

	// Print "pipe device driver registered with major number X"
	// to the kernel logging buffer so that:
	// - X is the obtained major number during registration
	// - Message printed using the informational log evel
	printk(KERN_INFO "pipe device driver registered with major number X\n");
	return 0;
}
/************/
static void pipe_exit(void)
{
    // deregister character device
	cdev_del(mycdev);

    // deregister the device driver's device numbers
	unregister_chrdev_region(mydev, 1);

	// print "pipe device driver deregistered" to the kernel
	// logginf buffer using the information log level
	printk(KERN_INFO "pipe device driver deregistered\n");
}
/************/
module_init(pipe_init);
module_exit(pipe_exit);