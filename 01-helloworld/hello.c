#include <linux/init.h>     /* For init macro */
#include <linux/kernel.h>   /* For KERN_INFO */
#include <linux/module.h>   /* For all modules */

/* The static keyword specify all the following variable and functions are only visible to this module */

static char *name = "johnnylord";

/* Module entry/exit method */
static int __init helloworld_init(void)
{
    pr_info("Hello world - %s\n", name);
    return 0;
}

static void __exit helloworld_exit(void)
{
    pr_info("Goodbye - %s\n", name);
    return;
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("johnnylord");
MODULE_DESCRIPTION("Hello world kernel module");
