# Hello World Kernel Module

Linux kernel provide feature of dynamically loading external code (loadable kernel module) into the kernel. To reach this goal, we need to define two essential methods in kernel module file (typically, a C program).

The start entry of kernel module is decorated by a `__init` modifier, and the exit entry of kernel module is decorated by a `__exit` modifer.
```c
static int __init init_func(void);
static void __exit exit_func(void);
``

Generally, the init function prepare the runtime environment of the kernel module, such as registering driver code and allocate resources. The exit function release the resource and do the the actions in reverse order as in the init function.

Then we use helper functions in kernel to register the start/exit entry of the kernel module.
```c
module_init(init_funct);
module_exit(exit_funct);
```

After that, you are ready to compile the kernel module against the kernel source tree and insert them into the kernel.
```bash
$ sudo insmod [module].ko # Insert kernel module
$ sudo rmmod [module].ko  # Remove kernel module

# Check existing loaded modules
$ sudo lsmod

# Check module description
$ sudo modinfo [module].ko
```

