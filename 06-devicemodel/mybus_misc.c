#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include "mybus.h"

MODULE_DESCRIPTION("MYBUS misc driver");
MODULE_AUTHOR("Kernel Hacker");
MODULE_LICENSE("GPL");

#define BUF_SIZE 1024

struct mybus_misc_device {
	struct miscdevice misc;
	struct mybus_device *dev;
	char buf[BUF_SIZE];
};

static int my_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t my_read(struct file *file, char __user *user_buffer,
		   size_t size, loff_t *offset)
{
	struct mybus_misc_device *bmd = (struct mybus_misc_device *)file->private_data;
	ssize_t len = min(sizeof(bmd->buf) - (ssize_t)*offset, size);

	if (len <= 0)
		return 0;

	if (copy_to_user(user_buffer, bmd->buf + *offset, len))
		return -EFAULT;

	*offset += len;
	return len;
}

static ssize_t my_write(struct file *file, const char __user *user_buffer,
		    size_t size, loff_t *offset)
{
	struct mybus_misc_device *bmd = (struct mybus_misc_device *)file->private_data;
	ssize_t len = min(sizeof(bmd->buf) - (ssize_t)*offset, size);

	if (len <= 0)
		return 0;

	if (copy_from_user(bmd->buf + *offset, user_buffer, len))
		return -EFAULT;

	*offset += len;
	return len;
}

struct file_operations mybus_misc_fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.read = my_read,
	.write = my_write,
	.release = my_release,
};

static int mybus_misc_count;

// Populate a misc device and register it to the system
int mybus_misc_probe(struct mybus_device *dev)
{
	struct mybus_misc_device *bmd;
	char buf[32];
	int ret;

	dev_info(&dev->dev, "%s: %s %d\n", __func__, dev->type, dev->version);

	if (dev->version > 1) {
		dev_info(&dev->dev, "unknown version: %d\n", dev->version);
		return -ENODEV;
	}

	bmd = kzalloc(sizeof(*bmd), GFP_KERNEL);
	if (!bmd)
		return -ENOMEM;

    // Initialize composite misc device
	bmd->misc.minor = MISC_DYNAMIC_MINOR;
	snprintf(buf, sizeof(buf), "mybus-misc-%d", mybus_misc_count++);
	bmd->misc.name = kstrdup(buf, GFP_KERNEL);
	bmd->misc.parent = &dev->dev;
	bmd->misc.fops = &mybus_misc_fops;
	bmd->dev = dev;
	dev_set_drvdata(&dev->dev, bmd);    // Bind private driver data to the device

    // Register misc device
	ret = misc_register(&bmd->misc);
	if (ret) {
		dev_err(&dev->dev, "failed to register misc device: %d\n", ret);
		return ret;
	}

	return 0;
}

// Free the misc device
void mybus_misc_remove(struct mybus_device *dev)
{
	struct mybus_misc_device *bmd;

	bmd = (struct mybus_misc_device *)dev_get_drvdata(&dev->dev);

	misc_deregister(&bmd->misc);
	kfree(bmd);
}

/* Driver information */
struct mybus_driver mybus_misc_driver = {
	.type = "misc",
	.probe = mybus_misc_probe,
	.remove = mybus_misc_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "mybus_misc",
	},
};

/* Module methods */
static int my_init(void)
{
	int err;

    pr_info("Register driver on the bus\n");
	err = mybus_register_driver(&mybus_misc_driver);
	if(err) {
		pr_err("unable to register driver: %d\n", err);
		return err;
	}

	return 0;
}

static void my_exit(void)
{
    pr_info("Unregister driver on the bus\n");
	mybus_unregister_driver(&mybus_misc_driver);
}

module_init(my_init);
module_exit(my_exit);
