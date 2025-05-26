#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include "buff_helper.h"

#include <asm/uaccess.h>

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
	.release = pipe_release,
	.llseek = no_llseek
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
	// Debug:

	nonseekable_open(inode,filp);

	printk(KERN_INFO "pipe_open(): Returning\n");
    return 0;
}
/************/
static int pipe_release(struct inode *inode, struct file *filp)
{
	// Debug:
    printk(KERN_INFO "pipe_release(): Returning\n");
    return 1;
}
/************/
int Retorno;
int echo_n;

ssize_t pipe_read(struct file *filep, char __user *buff, size_t count, loff_t *offp)
{
	char kern_buffer[BUFFER_MAX_SIZE];
	//echo_n = snprintf(pipe_buffer.data_space, sizeof(pipe_buffer.data_space));

	int i=0;
    for( i=0;count>i;i++){
		if(circ_buff_pop(&pipe_buffer,&kern_buffer[i]) == -1) break;
	}
	kern_buffer[i] = '\0';
	copy_to_user(buff,kern_buffer,i);
	// Debug:
	printk(KERN_INFO "pipe_read(): Read X characters\n");
	return i;
}
/************/
ssize_t pipe_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp)
{
    // Should write data from user space to circ_buffer.
	char kern_buffer[BUFFER_MAX_SIZE];

	copy_from_user(kern_buffer, buff, count);

	kern_buffer[count] = '\0';
	printk(KERN_INFO "mensagem: %s \n", kern_buffer);
	int i=0;
	for(i=0;count>;i++){
		circ_buff_push(&pipe_buffer,kern_buffer[i]);
	}

	// TODOs (Exercise 6):
	// The write() should take the a string content stored on buff (from the user memory space) and store it in a local buffer (in the kernel memory space).
	// The transferred content can be shown in a printk function for debugging.
	// Remember to add a zero (null) character at the end of the string to signal its termination.
	// It should also count the number of stored characters and update the total number of echoed characters, for latter use by the read() function.
	// Finally, the total number of written bytes must be returned.

	// REMEMBER TO NEVER RETURN A CONSTANT 0 VALUE, OR THE CALLER COULD RETRY THE WRITE OPERATION ENDLESSLY.
	echo_n += count;
	// DON'T FORGET TO NEVER RETURN A CONSTANT ZERO VALUE

	// Debug:
	printk(KERN_INFO "pipe_write(): Wrote X characters\n");
	return count?count:-1;
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


	if (return_of_alloc_chrdev_region < 0) {
		printk(KERN_ERR "Failed to register echo device driver\n");
			return return_of_alloc_chrdev_region;
		}

    // TODOs (Exercise 4): Create struture to represent char devices
	mycdev = cdev_alloc();
	mycdev->owner= THIS_MODULE;
	mycdev->ops = &myfops;

    // TODO (Exercise 4): Register character device into the kernel
	return_of_cdev_add= cdev_add(mycdev, mydev, 1);
	if (return_of_cdev_add < 0){
	 	printk(KERN_ERR "Failed to register character device\n");
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
	cdev_del(mycdev);
	unregister_chrdev_region(mydev, 1);

	// print "pipe device driver deregistered" to the kernel
	// logginf buffer using the information log level
	printk(KERN_INFO "pipe device driver deregistered\n");
}
/************/
module_init(pipe_init);
module_exit(pipe_exit);