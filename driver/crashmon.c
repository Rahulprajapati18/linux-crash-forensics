#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "crashmon"

static int major_number;

static char message[256] = "No crash recorded\n";


static int crashmon_open(
    struct inode *inode,
    struct file *file
)
{
    printk(KERN_INFO "crashmon: device opened\n");
    return 0;
}


static ssize_t crashmon_read(
    struct file *file,
    char __user *buffer,
    size_t length,
    loff_t *offset
)
{
    int bytes;

    bytes = strlen(message);


    if (*offset >= bytes)
        return 0;


    if (copy_to_user(
            buffer,
            message,
            bytes))
        return -EFAULT;


    *offset += bytes;

    return bytes;
}


static ssize_t crashmon_write(
    struct file *file,
    const char __user *buffer,
    size_t length,
    loff_t *offset
)
{

    if (length > 255)
        length = 255;


    if (copy_from_user(
            message,
            buffer,
            length))
        return -EFAULT;


    message[length] = '\0';


    printk(KERN_INFO
           "crashmon: crash information received\n");


    return length;
}


static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = crashmon_open,
    .read = crashmon_read,
    .write = crashmon_write
};


static int __init crashmon_init(void)
{

    major_number =
        register_chrdev(
            0,
            DEVICE_NAME,
            &fops
        );


    if (major_number < 0)
    {
        printk(KERN_ALERT
               "crashmon registration failed\n");

        return major_number;
    }


    printk(KERN_INFO
           "crashmon loaded with major number %d\n",
           major_number);


    return 0;
}


static void __exit crashmon_exit(void)
{

    unregister_chrdev(
        major_number,
        DEVICE_NAME
    );


    printk(KERN_INFO
           "crashmon unloaded\n");
}


module_init(crashmon_init);
module_exit(crashmon_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rahul");
MODULE_DESCRIPTION(
    "Linux Crash Forensics Character Device Driver"
);
