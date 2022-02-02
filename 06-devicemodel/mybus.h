#ifndef _MYBUS_H
#define _MYBUS_H

#include <linux/device.h>

/* Device that will be registered on the bus */
struct mybus_device {
	const char *type;
	int version;
	struct device dev; // Core device structure in linux device model */
};

/* Refer back to the wrapper device structure */
#define to_mybus_device(ptr) container_of(ptr, struct mybus_device, dev)

/* Device driver that will be registered on the bus */
struct mybus_driver {
	const char *type;

	int (*probe)(struct mybus_device *dev);
	void (*remove)(struct mybus_device *dev);

	struct device_driver driver; // Core driver structure in linux device model
};

#define to_mybus_driver(ptr) container_of(ptr, struct mybus_driver, driver)

/* Helper functions for driver developer to register their drivers to the bus */
int mybus_register_driver(struct mybus_driver *drv);
void mybus_unregister_driver(struct mybus_driver *drv);

#endif
