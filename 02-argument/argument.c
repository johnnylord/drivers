#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>


// Initial values when no arguments passed in the kernel
static char *name = "johnnylord";
static int arrs[2] = { 10, 100 };
static int arrs_argc = 0;

// module_param(foo, int, 0000);
// - first argument is the variable name of parameter
// - second argument is its data type
// - third argument is the permission bits
module_param(name, charp, 0000);
MODULE_PARM_DESC(name, "Name of user");

// module_param_array(name, type, num, perm);
// - first argument is the variable name of parameter
// - second argument is the data type of elements of the array
// - third argument is a pointer to the variable that will store te number of passing arguments.
// - fourth argument is the permission bits
module_param_array(arrs, int, &arrs_argc, 0000);
MODULE_PARM_DESC(arrs, "An array of integers");


static int __init argument_init(void)
{
    int i;

    // Print out passed arguements here
    pr_info("My name is %s\n", name);
    pr_info("I got an array of integers\n");
    for (i = 0; i < (sizeof arrs / sizeof(int)); i++)
        pr_info("\t%d argument is %d\n", i, arrs[i]);
    return 0;
}

static void __exit argument_exit(void)
{
    pr_info("Goodbye - %s\n", name);
    return;
}

module_init(argument_init);
module_exit(argument_exit);
MODULE_LICENSE("GPL");
