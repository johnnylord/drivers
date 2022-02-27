#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/slab.h>

#include "mybus.h"

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Kernel Hacker");
MODULE_DESCRIPTION ("MYBUS bus module");

/* Check if the populated device can be associated with drivers */
static int mybus_match(struct device *dev, struct device_driver *driver)
{
	struct mybus_device *mybus_dev = to_mybus_device(dev);
	struct mybus_driver *mybus_drv = to_mybus_driver(driver);

	if (!strcmp(mybus_dev->type, mybus_drv->type)) {
        pr_info("Bus match device %s with driver %s\n", mybus_dev->type, mybus_drv->type);
		return 1;
    }

	return 0;
}

/* Initialize attached device */
static int mybus_probe(struct device *dev)
{
	struct mybus_device *mybus_dev = to_mybus_device(dev);
	struct mybus_driver *mybus_drv = to_mybus_driver(dev->driver);

    pr_info("Bus perform probe on device %s\n", dev_name(dev));
	return mybus_drv->probe(mybus_dev);
}

/* Relase unattached device */
static int mybus_remove(struct device *dev)
{
	struct mybus_device *mybus_dev = to_mybus_device(dev);
	struct mybus_driver *mybus_drv = to_mybus_driver(dev->driver);

    pr_info("Bus perform remove on device %s\n", dev_name(dev));
	mybus_drv->remove(mybus_dev);
	return 0;
}

/* Bus attributes & operations */
static int mybus_add_dev(const char *name, const char *type, int version);

static ssize_t
add_store(struct bus_type *bt, const char *buf, size_t count)
{
	char type[32], name[32];
	int version;
	int ret;

	ret = sscanf(buf, "%31s %31s %d", name, type, &version);
	if (ret != 3) {
        pr_err("Invalid input for bus add\n");
		return -EINVAL;
    }

	return mybus_add_dev(name, type, version) ? 0 : count;
}
BUS_ATTR_WO(add);

static int mybus_del_dev(const char *name);

static ssize_t
del_store(struct bus_type *bt, const char *buf, size_t count)
{
	char name[32];
	int version;

	if (sscanf(buf, "%s", name) != 1) {
        pr_err("Invalid input for bus del\n");
		return -EINVAL;
    }

	return mybus_del_dev(name) ? 0 : count;
}
BUS_ATTR_WO(del);

static struct attribute *mybus_bus_attrs[] = {
	&bus_attr_add.attr,
	&bus_attr_del.attr,
	NULL // For termination
};
ATTRIBUTE_GROUPS(mybus_bus);

struct bus_type mybus_bus_type = {
	.name	= "mybus",
	.match	= mybus_match,
	.probe  = mybus_probe,
	.remove  = mybus_remove,
	.bus_groups = mybus_bus_groups,
};

/* Device attributes and associated operations */
static ssize_t
type_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mybus_device *mybus_dev = to_mybus_device(dev);

    pr_info("Extract device type %s\n", dev_name(dev));
	return sprintf(buf, "%s\n", mybus_dev->type);
}
DEVICE_ATTR_RO(type); // Create variable => dev_attr_type

static ssize_t
version_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mybus_device *mybus_dev = to_mybus_device(dev);

    pr_info("Extract device version %s\n", dev_name(dev));
	return sprintf(buf, "%d\n", mybus_dev->version);
}
DEVICE_ATTR_RO(version); // Create variable => dev_attr_version

static struct attribute *mybus_dev_attrs[] = {
	&dev_attr_type.attr,
	&dev_attr_version.attr,
	NULL // For loop Termination
};
ATTRIBUTE_GROUPS(mybus_dev);

static int mybus_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    pr_info("Set device uevent %s\n", dev_name(dev));
	return add_uevent_var(env, "MODALIAS=mybus:%s", dev_name(dev));
}

static void mybus_dev_release(struct device *dev)
{
	struct mybus_device *mybus_dev = to_mybus_device(dev);

    pr_info("Release device %s\n", dev_name(dev));
	kfree(mybus_dev->type);
	kfree(mybus_dev);
}

// Device default attributes & handlers
struct device_type mybus_device_type = {
	.groups = mybus_dev_groups,
	.uevent	= mybus_dev_uevent,
	.release = mybus_dev_release,
};

/* Module private method to populate device on the bus */
static int mybus_add_dev(const char *name, const char *type, int version)
{
    int ret;
    struct device *dev;
	struct mybus_device *mybus_dev;

    // Allocate device memory from kernel lowmem region
	mybus_dev = kzalloc(sizeof(*mybus_dev), GFP_KERNEL);
	if (!mybus_dev) {
        pr_err("Fail to allocate space for device %s\n", name);
		return -ENOMEM;
    }

    // Initialize device data with specified values
	mybus_dev->type = kstrdup(type, GFP_KERNEL);
	mybus_dev->version = version;

    // Attach device to the bus
	mybus_dev->dev.bus = &mybus_bus_type;
	mybus_dev->dev.type = &mybus_device_type;
	mybus_dev->dev.parent = NULL;

    // Set the unique device name with the device
	dev_set_name(&mybus_dev->dev, "%s", name);

	ret = device_register(&mybus_dev->dev);
    if (ret) {
        pr_err("Fail to add device %s\n", name);
        return ret;
    }
    return 0;
}

static int mybus_del_dev(const char *name)
{
	struct device *dev;

    // Find the reference of device with specified name
	dev = bus_find_device_by_name(&mybus_bus_type, NULL, name);
	if (!dev) {
        pr_err("Fail to delete device %s\n", name);
		return -EINVAL;
    }

    // Unregister device & decrease the reference counter
    pr_info("Delete device %s\n", dev_name(dev));
	device_unregister(dev);
	put_device(dev);

	return 0;
}

/* Module exported methods */
int mybus_register_driver(struct mybus_driver *drv)
{
	int ret;

    // Attached driver on the bus
	drv->driver.bus = &mybus_bus_type;
	ret = driver_register(&drv->driver);
	if (ret) {
        pr_err("Fail to register driver %s\n", drv->type);
		return ret;
    }

    pr_info("Register driver %s\n", drv->type);
	return 0;
}
EXPORT_SYMBOL(mybus_register_driver);

void mybus_unregister_driver(struct mybus_driver *drv)
{
	driver_unregister(&drv->driver);
    pr_info("Unregister driver %s\n", drv->type);
}
EXPORT_SYMBOL(mybus_unregister_driver);

/* Module privated methods */
static int __init my_bus_init(void)
{
	int ret;

    // Register bus into the kernel (device & driver will later be attached on the bus)
    pr_info("Register mybus\n");
	ret = bus_register(&mybus_bus_type);
	if (ret < 0) {
		pr_err("Unable to register bus\n");
		return ret;
	}

	return 0;
}

static void my_bus_exit(void)
{
    // Unregister bus in the kernel
    pr_info("Unregister mybus\n");
	bus_unregister(&mybus_bus_type);
}

module_init(my_bus_init);
module_exit(my_bus_exit);
