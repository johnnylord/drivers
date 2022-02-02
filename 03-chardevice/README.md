# Kernel Device

Everything are represented as files in linux kernel. In linux kernel, devices can be categorized as block device (e.g. disk) and character device (e.g. network device). Block devices are typically realized with buffering/cache and they must be random access. However, character devices usually have no buffering and they are sequentially accessed.

## NOTE
- Each device can be categorized with a class. (`class_create` will create a /sys/class/[class-name] in the sysfs)
- Each character device is named with two numbers (major number and minor number) major usually specify the driver ID, and minor indicate the device ID.
- 
