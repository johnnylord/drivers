#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "message.h"

#define DEVICE_NAME "ioctl"

static int major;
static struct class *cls;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static long device_ioctl(struct file *, unsigned int, unsigned long);

static struct file_operations device_fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_release,
	.compat_ioctl = device_ioctl,
	.unlocked_ioctl = device_ioctl,
};

int __init mymodule_init(void)
{
	major = register_chrdev(0, DEVICE_NAME, &device_fops);
	if (major < 0) {
		pr_info("init: unable to register character device\n");
		return -ENODEV;
	}
	cls = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

	return 0;
}

void __exit mymodule_exit(void)
{
	device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	unregister_chrdev(major, DEVICE_NAME);
	return;
}

static int device_open(struct inode *inode, struct file *file)
{
	try_module_get(THIS_MODULE);
	return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);
	return 0;
}

static long device_ioctl(struct file *file, unsigned int cmd, unsigned long addr)
{
	int i;
	unsigned long sum = 0, item1 = 0, item2 = 1;
	ssize_t size = sizeof(struct message);
	struct message *msg = kmalloc(size, GFP_KERNEL);

	copy_from_user(msg, (void*)addr, size);

	pr_info("ioctl: cmd=%u\n", cmd);
	pr_info("ioctl: order=%u\n", msg->order);

	// Simple Fibnoacci Algorithm
	if (msg->order == 0) {
		msg->value = 0;
	} else if (msg->order == 1) {
		msg->value = 1;
	} else {
		for (i=2; i<=msg->order; i++) {
			sum = item1 + item2;
			item1 = item2;
			item2 = sum;
		}
	}
	msg->value = sum;

	copy_to_user((void*)addr, msg, size);
	return 0;
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("johnnylord");
MODULE_DESCRIPTION("ioctl control API");
