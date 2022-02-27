#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROCFS_NAME "procfs"

struct proc_dir_entry *proc_file;

static ssize_t procfile_read(struct file *, char *, size_t, loff_t *);

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
};
#endif

/* Module Method */
static int __init procfs_init(void)
{
    // Create Proc file entry under /proc
    proc_file = proc_create(PROCFS_NAME, 0644, NULL, &proc_file_fops);
    if (proc_file == NULL) {
        proc_remove(proc_file);
        pr_alert("Fail to create /proc/%s\n", PROCFS_NAME);
        return -ENOMEM;
    }

    pr_info("/proc/%s created.\n", PROCFS_NAME);
    return 0;
}

static void __exit procfs_exit(void)
{
    proc_remove(proc_file);
    pr_info("/proc/%s removed.\n", PROCFS_NAME);
}

/* Proc FS method */
static ssize_t procfile_read(struct file *filp,
                            char *buffer,
                            size_t buffer_length,
                            loff_t *offset)
{
    char s[13] = "HelloWorld!\n";
    int len = sizeof(s);
    ssize_t ret = len;

    if (*offset >= len || copy_to_user(buffer, s, len)) {
        pr_info("copy_to_user failed\n");
        ret = 0;
    } else {
        pr_info("procfile read %s\n",
                filp->f_path.dentry->d_name.name);
        *offset += len;
    }
    return ret;
}

module_init(procfs_init);
module_exit(procfs_exit);
MODULE_LICENSE("GPL");
