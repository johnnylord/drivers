#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>


#define DEVICE_NAME "hello"

/* Folloing information are devices specific */
#define HELLO_MEM_START_ADDRESS 0x100000
#define HELLO_MEM_END_ADDRESS   0x1FFFFF
#define HELLO_IRQNUM   6

/* Specify my deivce resources information */
/*
 * static struct resource hello_resources[] = {
 *     {
 *         .start  = HELLO_MEM_START_ADDRESS,
 *         .end    = HELLO_MEM_END_ADDRESS,
 *         .flags  = IORESOURCE_MEM,
 *     },
 *     {
 *         .start  = HELLO_IRQNUM,
 *         .end    = HELLO_IRQNUM,
 *         .flags  = IORESOURCE_IRQ,
 *     }
 *  };
 */

static struct platform_device *pdev;

static int platformdevice_init(void)
{
    int err;

    pr_info("Register Platform Device of name 'hello'\n");

    // Allocate platform device object
    pdev = platform_device_alloc(DEVICE_NAME, 0); // This will assign default `release` function to the device
    if (!pdev) {
        pr_alert("%s(#%d): Fail to perform platform_device_alloc", __func__, __LINE__);
        return -ENOMEM;
    }
    /* pdev->resource = hello_resources; */
    /* pdev->num_resources = ARRAY_SIZE(hello_resources); */

    // Register platform device object
    err = platform_device_add(pdev);
    if (err) {
        pr_alert("%s(#%d): Fail to perform platform_device_add", __func__, __LINE__);
        goto dev_add_failed;
    }

    return 0;

dev_add_failed:
    platform_device_unregister(pdev);
    return err;
}

static void platformdevice_exit(void)
{
    pr_info("Ungister Platform Device of name 'hello'\n");
    platform_device_unregister(pdev);
}

module_init(platformdevice_init);
module_exit(platformdevice_exit);
MODULE_LICENSE("GPL");
