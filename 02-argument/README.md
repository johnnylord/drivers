# Module Arguments

Just like most programs can accept input arguments from user, kernel module can also accept arguments specified by user.
```c
#include <linux/module.h>
#include <linux/moduleparam.h>

// Variables that would be exported as arguments to user
static char *name = "johnnylord";
static int arrs[2] = { 10, 100 };
static int arrs_argc = 0;

// Specify the permission and type of the variable exported to the user
module_param(name, charp, 0000);
MODULE_PARM_DESC(name, "Name of user");

// Specify the permission and type of the variable exported to the user
module_param_array(arrs, int, &arrs_argc, 0000);
MODULE_PARM_DESC(arrs, "An array of integers");
```

To check which arguments can be specified by the user, you can use `modinfo` to check the information
```bash
$ modinfo [module].ko
```
