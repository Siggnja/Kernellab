/*
 * Kernellab
 */
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include "pidinfo.h"

/* - kcurrent_write()
 * Fetch the memory address from the kcurrent file.
 * Get the current pid, store in int var.
 * Use copy_to_user to store the pid in user memory space.
 */
static ssize_t kcurrent_write(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	void *pBuf = (void *)simple_strtol(buf, NULL, 16);

	int pid = (int)current->pid;

	if(copy_to_user(pBuf, (void *)&pid, sizeof(int)) != 0)
		return EFAULT;

	/* Your code here. */
	printk("Current PID: %d\n", current->pid);
	printk("Current Commandi: %s\n", current->comm);
	printk("The contents in the memory location are: %d\n", *(int *)pBuf);
	printk("\n");

	return count;
} 

static struct kobj_attribute kcurrent_attribute =
__ATTR(kcurrent, 0220, NULL, kcurrent_write);


/* - pid_write()
 * Malloc space in kernel to receive the sysfs from user space.
 * Fetch the memory address from the PID file.
 * Use copy_from_user to fetch the sysfs message struct to kernel memory.
 * Using task we populate the pid_info struct.
 * We copy the pid_info struct into user memory again.
 * Free the memory used for kmP.
 */
static ssize_t pid_write(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	struct sysfs_message *kmP = kmalloc(sizeof(struct sysfs_message), GFP_KERNEL);

	void *pBuf = (void *)simple_strtol(buf, NULL, 16);

	if(copy_from_user(kmP, pBuf, sizeof(struct sysfs_message)) != 0)
		return EFAULT;

	struct pid_info info;
	struct task_struct *task;

	for_each_process(task) {
		if (task->pid == kmP->pid) {
			info.pid = task->pid;
			sscanf(task->comm, "%s", &(info.comm));
			info.state = task->state;
		}
	}   
	
	struct pid_info *p = &info;

	if(copy_to_user((struct pid_info *)kmP->address, p, sizeof(info)))
		return EFAULT;
	
	kfree(kmP);
	return count;
}

static struct kobj_attribute pid_attribute =
	__ATTR(pid, 0220, NULL, pid_write);

/* Setup  */
static struct attribute *attrs[] = {
	&kcurrent_attribute.attr,
	&pid_attribute.attr,
	NULL,
};
static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *kernellab_kobj;

static int __init kernellab_init(void)
{
    int retval;
    kernellab_kobj = kobject_create_and_add("kernellab", kernel_kobj);
    if (!kernellab_kobj)
        return ENOMEM;

    retval = sysfs_create_group(kernellab_kobj, &attr_group);
    if (retval)
        kobject_put(kernellab_kobj);

    struct task_struct *task;
    for_each_process(task) {
        if (task->pid == 1) {
            printk("kernellab module INJECTED\n");
            printk("Init procces: %s\n", task->comm);
        }
    }   
    return retval;
}

static void __exit kernellab_exit(void)

{
    printk("kernellab module UNLOADED\n");

    kobject_put(kernellab_kobj);
}

module_init(kernellab_init);
module_exit(kernellab_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hordur Ragnarsson <hordur12@ru.is");
MODULE_AUTHOR("Kari Mimisson <kari13@ru.is");
