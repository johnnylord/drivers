#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>

#define DRIVER_NAME "hello"

static int hello_probe(struct platform_device *pdev)
{
    pr_info("hello_probe get called when platform device matched with this driver\n");
    pr_info("Deivce name: '%s'\n", pdev->name);
    return 0;
}

static int hello_remove(struct platform_device *pdev)
{
    pr_info("hello_remove get called when matched platform device is removed\n");
    pr_info("Deivce name: '%s'\n", pdev->name);
    return 0;
}

static const struct of_device_id hello_of_match[] = {
    { .compatible = "hello" },
    { },
};

static struct platform_driver hello_driver =
{
    .probe = hello_probe,
    .remove = hello_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = hello_of_match,
    }
};

static int platformdriver_init(void)
{
    pr_info("Register Platform driver of name 'hello'\n");
    return platform_driver_register(&hello_driver);
}

static void platformdriver_exit(void)
{
    pr_info("Unregister Platform driver of name 'hello'\n");
    platform_driver_unregister(&hello_driver);
}

module_init(platformdriver_init);
module_exit(platformdriver_exit);
MODULE_LICENSE("GPL");
