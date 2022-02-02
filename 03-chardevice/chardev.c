#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUF_LEN 80

/* Prototypes - this would normally placed in header files */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations chardev_fops = {
    .read = device_read,        // Called when perform read on the device file
    .write = device_write,      // Called when perform write on the device file
    .open = device_open,        // Called when a process tries to open the device file (e.g. sudo cat /dev/chardev)
    .release = device_release,  // Called when a process close the file
};

/* Global Variables are declared as static, so are global within the file */
static int major;
static int open_device_cnt = 0;
static char msg[BUF_LEN];
static char *msg_ptr;
static struct class *cls;

/* Module Method */
static int __init chardev_init(void)
{
    /* Register device handlers to the kernel */
    major = register_chrdev(0, DEVICE_NAME, &chardev_fops);
    if (major < 0) {
        pr_alert("Registering char device failed with %d\n", major);
        return major;
    }
    pr_info("Device driver major number is %d\n", major);

    /* Create device file under /dev/ directory */
    cls = class_create(THIS_MODULE, DEVICE_NAME); // Subdirectory(class) under sysfs, class name is DEVICE NAME

    // Leaf node (file) in sysfs, device name is DEVICE_NAME
    // MKDEV convert major and minor number to a combined number
    //  - struct class *cls
    //  - struct device *parent
    //  - dev_t devt
    //  - void *drvdata
    //  - const char *fmt
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    pr_info("Device created on /dev/%s\n", DEVICE_NAME);

    return SUCCESS;
}

static void __exit chardev_exit(void)
{
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, DEVICE_NAME);
}

/* Device Method */
static int device_open(struct inode *inode, struct file *file)
{
    static int counter = 0;
    if (open_device_cnt)
        return -EBUSY;

    open_device_cnt++;
    sprintf(msg, "I already told %d times hello worlds!\n", counter++);
    msg_ptr = msg;
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
    open_device_cnt--;
    module_put(THIS_MODULE);
    return SUCCESS;
}

static ssize_t device_read(struct file *filp,   // see include/linux/fs.h
                        char *buffer,           // buffer to fill the data
                        size_t length,          // length of data to read
                        loff_t *offset)
{
    int bytes_read = 0;

    if (*msg_ptr == 0)
        return 0;

    /* Put the data into the buffer */
    while (length && *msg_ptr) {
        /* The buffer is in the user data segmen, not the kernel segment */
        /* so "*" assignment won't work. We have to use put_user which */
        /* copies data from the kernel data segment to the user data segment */
        put_user(*(msg_ptr++), buffer++);
        length--;
        bytes_read++;
    }

    return bytes_read;
}

static ssize_t device_write(struct file *filp,
                            const char *buff,
                            size_t len,
                            loff_t *off)
{
    pr_alert("Sorry, this operation is not supported.\n");
    return -EINVAL;
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("johnnylord");
MODULE_DESCRIPTION("Character Device");
