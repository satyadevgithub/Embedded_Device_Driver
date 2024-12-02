#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>  
#include <linux/string.h>
#include "alpha_ioctls.h"

#define MY_CLASS_NAME	"cdac_cls"
#define MY_DEV_NAME	"cdac_dev"

dev_t dev = 0;
static struct class *dev_class;
static struct device *cdevice;
static struct cdev my_cdev;

struct my_node {
    char name[10];
    unsigned long id;
    bool sex;    // true = Male, false = Female
    bool status; // true = Permanent, false = Contractor
    struct list_head my_list;
};

LIST_HEAD(my_head);

static int my_open(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *off);
static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int my_release(struct inode *inode, struct file *file);
static void __exit my_mod_exit(void);
static int __init my_mod_init(void);

static int my_open(struct inode *inode, struct file *file)
{
    pr_info("My open function %s called\n", __func__);
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    pr_info("My read function %s called\n", __func__);
    return 0;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    struct my_node temp_node;
    struct my_node *new_node;

    pr_info("Write function called\n");
    
    if (len != sizeof(temp_node)) {
        pr_err("Invalid buffer length\n");
        return -EINVAL;
    }
    
    if (copy_from_user(&temp_node, buf, sizeof(temp_node))) {
        pr_err("Failed to copy data from user space\n");
        return -EFAULT;
    }
    
    new_node = kmalloc(sizeof(struct my_node), GFP_KERNEL);
    if (!new_node) {
        pr_err("Failed to allocate memory for node\n");
        return -ENOMEM;
    }

    strncpy(new_node->name, temp_node.name, sizeof(new_node->name) - 1);
    new_node->name[sizeof(new_node->name) - 1] = '\0'; // Null-terminate
    new_node->id = temp_node.id;
    new_node->sex = temp_node.sex;
    new_node->status = temp_node.status;

    INIT_LIST_HEAD(&new_node->my_list);
    list_add(&new_node->my_list, &my_head);

    pr_info("Added node: Name = %s, ID = %lu\n", new_node->name, new_node->id);
    return len; // Return the number of bytes written
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct my_node *node, *tmp;
    
    switch (cmd) {
    case SHOW_LIST:
        pr_info("Displaying Employee List:\n");
        list_for_each_entry(node, &my_head, my_list) {
            pr_info("Name: %-10s | ID: %-10lu | Sex: %-5c | Status: %-10s\n",
                    node->name,
                    node->id,
                    node->sex ? 'M' : 'F',
                    node->status ? "Permanent" : "Contractor");
        }
        break;

    case DEL_LIST:
       pr_info("Deleting all nodes in the Employee List...\n");
        list_for_each_entry_safe(node, tmp, &my_head, my_list) {
            pr_info("Deleting node: Name = %s\n", node->name);
            list_del(&node->my_list);
            kfree(node);
        }
        break;
        
    default:
        pr_err("Undefined ioctl command: %u\n", cmd);
        return -EINVAL;
    }
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    pr_info("Release function called\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .read = my_read,
    .write = my_write,
    .unlocked_ioctl = my_ioctl,
    .release = my_release,
};

static int __init my_mod_init(void)
{
    int ans;
    pr_info("Hello world from %s!\n", KBUILD_MODNAME);

    ans = alloc_chrdev_region(&dev, 0, 1, MY_DEV_NAME);
    if (ans < 0) {
        pr_err("Error in major:minor allotment!\n");
        return -1;
    }
    pr_info("major:minor %d:%d allotted!\n", MAJOR(dev), MINOR(dev));
    
    cdev_init(&my_cdev, &fops);
    ans = cdev_add(&my_cdev, dev, 1);
    if (ans < 0) {
        pr_err("Could not add cdev to the kernel!\n");
        goto r_cdev;
    }

    dev_class = class_create(THIS_MODULE, MY_CLASS_NAME);
    if (IS_ERR(dev_class)) {
        pr_err("Could not create device class %s\n", MY_CLASS_NAME);
        goto r_class;
    }

    cdevice = device_create(dev_class, NULL, dev, NULL, MY_DEV_NAME);
    if (IS_ERR(cdevice)) {
        pr_err("Could not create device %s\n", MY_DEV_NAME);
        goto r_device;
    }
    pr_info("Device %s under class %s created with success\n", MY_DEV_NAME, MY_CLASS_NAME);

    return 0;

r_device:
    class_destroy(dev_class);
r_class:
    cdev_del(&my_cdev);
r_cdev:
    unregister_chrdev_region(dev, 1);

    return -1;
}

static void __exit my_mod_exit(void)
{
    struct my_node *node, *tmp;

    pr_info("Exiting module: %s\n", KBUILD_MODNAME);
    
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Released major:minor numbers\n");

    list_for_each_entry_safe(node, tmp, &my_head, my_list)
    {
        pr_info("Deleting Employee node with Name = %s\n", node->name);
        list_del(&node->my_list);
        kfree(node);
    }
	
    pr_info("Module %s removed successfully\n", KBUILD_MODNAME);
}


module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EDD <edd@cdac.gov.in>");
MODULE_DESCRIPTION("Kernel module with linked list and IOCTL");

