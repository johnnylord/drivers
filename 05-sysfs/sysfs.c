#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/sysfs.h>


static struct kobject *mymodule;

/* The variable you want to be able to change */
static int myvar = 0;

static ssize_t myvar_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", myvar);
}

static ssize_t myvar_store(struct kobject *kobj, struct kobj_attribute *attr, char *buf, size_t count)
{
    sscanf(buf, "%du", &myvar);
    return count;
}

// Specify handlers and file permission of variable in sysfs
static struct kobj_attribute myvar_attr = __ATTR(myvar, 0660, myvar_show, (void*)myvar_store);

/* Module Method */
static int __init mymodule_init(void)
{
    int error = 0;

    pr_info("mymodule: initialized\n");
    mymodule = kobject_create_and_add("mymodule", kernel_kobj);

    if(!mymodule)
        return -ENOMEM;

    // Associate kernel object with exported variable
    error = sysfs_create_file(mymodule, &myvar_attr.attr);
    if (error) {
        pr_info(
            "Failed to create the myvar file"
            "in /sys/kernel/mymodule\n");
    }
    return error;
}

static void __exit mymodule_exit(void)
{
    pr_info("mymodule: Exit success\n");
    kobject_put(mymodule);
}

module_init(mymodule_init);
module_exit(mymodule_exit);
MODULE_LICENSE("GPL");
