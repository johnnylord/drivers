#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <asm/io.h>
#include <linux/mm.h>
#include <linux/slab.h>

#define MB 1024*1024
#define DEVICE_NAME "kmem"

/* Module private members */
static int major;
static struct class *cls;
static char *reserve_virt_addr;

/* Module arguments */
static unsigned mem_size = 1;
module_param(mem_size, uint, 0000);
MODULE_PARM_DESC(mem_size, "size of reserved memory in MB");

/* File operations for mydevice */
static int device_open(struct inode *inode, struct file *file);
static int device_mmap(struct file *file, struct vm_area_struct *vma);
static int device_release(struct inode *inode, struct file *file);

static struct file_operations device_fops = {
	.owner = THIS_MODULE,
    .open = device_open,
    .mmap = device_mmap,
    .release = device_release,
};

static int __init mymodule_init(void)
{
	int i;

	// Register character device
	major = register_chrdev(0, DEVICE_NAME, &device_fops);
	if (major < 0) {
		pr_info("init: unable to register character device\n");
		return -EIO;
	}
	// Create device under /dev/
    cls = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

	// Logging
	pr_info("init: mem_size: %u MB\n", mem_size);
	pr_info("init: device major number %d\n", major);
    pr_info("init: device created at /dev/%s\n", DEVICE_NAME);
	pr_info("init: high memory physical address 0x%llx\n", virt_to_phys(high_memory));

	// Allocate memory space for memory mapping
	reserve_virt_addr = kmalloc(mem_size*MB, GFP_KERNEL);
	pr_info("init: virtual address of reserved memory 0x%lx\n", (unsigned long)reserve_virt_addr);
	pr_info("init: physical address of reserved memory 0x%lx\n", (unsigned long)virt_to_phys(reserve_virt_addr));

	// Initialize reserved memory space
	if (reserve_virt_addr) {
		for (i=0; i<mem_size*MB; i+=4) {
			reserve_virt_addr[i] = 'a';
			reserve_virt_addr[i+1] = 'b';
			reserve_virt_addr[i+2] = 'c';
			reserve_virt_addr[i+3] = 'd';
		}
	} else {
		goto fail;
	}

	return 0;

fail:
	// Cleanup allocated device
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
	unregister_chrdev(major, DEVICE_NAME);
	pr_info("init: unregister the device\n");
	return -ENODEV;
}

static void __exit mymodule_exit(void)
{
	if (reserve_virt_addr) {
		kfree(reserve_virt_addr);
		pr_info("exit: free the reserved space\n");
	}
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
	unregister_chrdev(major, DEVICE_NAME);
	pr_info("exit: unregister the device\n");
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

static int device_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long size = vma->vm_end - vma->vm_start;
	phys_addr_t reserve_phys_addr = virt_to_phys(reserve_virt_addr);

	if (size > mem_size*MB) {
		pr_info("mmap: size too big\n");
		return -ENXIO;
	}

	// Create page table entry for memory mapping
	vma->vm_flags |= VM_LOCKED;
	if (remap_pfn_range(vma,
				vma->vm_start,					// vritual memory address of user space
				reserve_phys_addr >> PAGE_SHIFT,// page frame number of physical memory address
				size,							// memory mapping size
				vma->vm_page_prot)				// flags for mapping space
	) {
		pr_info("mmap: remap page range failed\n");
		return -ENXIO;
	}

	return 0;
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("johnnylord");
MODULE_DESCRIPTION("mmap Device");
