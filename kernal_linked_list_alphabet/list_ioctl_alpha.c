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
#include <linux/ctype.h>

#include "alpha_ioctls.h"

// Number of nodes in the kernel linked list
#define MY_CLASS_NAME	"cdac_cls"
#define MY_DEV_NAME	"cdac_dev"


dev_t dev = 0;
static struct class *dev_class;
static struct device *cdevice;
static struct cdev my_cdev;

// Create a fops struct
static int my_open(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *off);
static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int my_release(struct inode *inode, struct file *file);
static inline int is_vowel(char c);
static void __exit my_mod_exit(void);
static int __init my_mod_init(void);


// Linked list node definition
struct my_node {
    char data;
    struct list_head my_list;
    struct list_head my_list1;
};

// Create list head pointer/node
LIST_HEAD(ALPHA_head);
LIST_HEAD(VOWEL_head);
LIST_HEAD(CONSO_head);

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
    char temp;
    pr_info("My write function %s called\n", __func__);
    
    // Allocate memory for the node
    struct my_node *node;
    node = (struct my_node *)kmalloc(sizeof(struct my_node), GFP_KERNEL);
    if (!node) {
        pr_err("Failed to allocate memory for node\n");
        return -ENOMEM;
    }

    // Ensure that only one byte is copied from user space
    if (len == 1) {
        if (copy_from_user((void *)&node->data, buf, sizeof(char))) {
            pr_err("Failed to copy data from user space\n");
            kfree(node);
            return -EFAULT;
        }
    } else {
        pr_err("Invalid buffer length\n");
        kfree(node);
        return -EINVAL;
    }

    // Process the character and initialize the list head
    temp = node->data;
    INIT_LIST_HEAD(&node->my_list);
    INIT_LIST_HEAD(&node->my_list1);
    
    	list_add(&node->my_list1, &ALPHA_head);
    // Add the node to the appropriate list (vowel or consonant)
    if (is_vowel(temp)) {
        pr_info("Vowel detected: %c\n", temp);
        list_add(&node->my_list, &VOWEL_head);
    } else {
        pr_info("Consonant detected: %c\n", temp);
        list_add(&node->my_list, &CONSO_head);
    }

    return len;
}

// Helper function to check if a character is a vowel
static inline int is_vowel(char c)
{
    c = tolower(c);
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}


static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct my_node *node, *tmp;
    
    switch (cmd) {
    case SHOW_LIST_ALPHA:
        pr_info("Displaying Alphabet linked list: ");
        list_for_each_entry(node, &ALPHA_head, my_list1)
        {
            pr_cont("%c", node->data);
            if (node->my_list1.next != &ALPHA_head)  // Check if it's not the last node
                pr_cont(", ");
        }
        pr_info("\n");
        break;

    case SHOW_LIST_VOWEL:
        pr_info("Displaying Vowel linked list: ");
        list_for_each_entry(node, &VOWEL_head, my_list)
        {
            pr_cont("%c", node->data);
            if (node->my_list.next != &VOWEL_head)  // Check if it's not the last node
                pr_cont(", ");
        }
        pr_info("\n");
        break;
        
    case SHOW_LIST_CONSO:
        pr_info("Displaying Consonent linked list: ");
        list_for_each_entry(node, &CONSO_head, my_list)
        {
            pr_cont("%c", node->data);
            if (node->my_list.next != &CONSO_head)  // Check if it's not the last node
                pr_cont(", ");
        }
        pr_info("\n");
        break;
    case DEL_LIST_ALPHA:
       pr_info("Deleting all nodes in the Alphabet linked list...\n");

        // Traverse the list and delete all nodes
        list_for_each_entry_safe(node, tmp, &ALPHA_head, my_list1)
        {
            pr_info("Deleting node with data = %d\n", node->data);
            list_del(&node->my_list1);  // Remove node from the list
            kfree(node);               // Free the node
        }
        break;
    case DEL_LIST_VOWEL:
       pr_info("Deleting all nodes in the Vowel linked list...\n");

        // Traverse the list and delete all nodes
        list_for_each_entry_safe(node, tmp, &VOWEL_head, my_list)
        {
            pr_info("Deleting node with data = %d\n", node->data);
            list_del(&node->my_list);  // Remove node from the list
            kfree(node);               // Free the node
        }
        break;
    case DEL_LIST_CONSO:
       pr_info("Deleting all nodes in the Consonent linked list...\n");

        // Traverse the list and delete all nodes
        list_for_each_entry_safe(node, tmp, &CONSO_head, my_list)
        {
            pr_info("Deleting node with data = %d\n", node->data);
            list_del(&node->my_list);  // Remove node from the list
            kfree(node);               // Free the node
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
    pr_info("My release function %s called\n", __func__);
    return 0;
}

// Create a fops struct
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
    //struct my_node *node;

    pr_info("Hello world from %s!\n", KBUILD_MODNAME);

    // Allocate char device region
    ans = alloc_chrdev_region(&dev, 0, 1, MY_DEV_NAME);
    if (ans < 0) {
        pr_err("Error in major:minor allotment!\n");
        return -1;
    }
    pr_info("major:minor %d:%d allotted!\n", MAJOR(dev), MINOR(dev));

    // Initialize the cdev structure
    cdev_init(&my_cdev, &fops);

    ans = cdev_add(&my_cdev, dev, 1);
    if (ans < 0) {
        pr_err("Could not add cdev to the kernel!\n");
        goto r_cdev;
    }

    // Create device class
    dev_class = class_create(THIS_MODULE, MY_CLASS_NAME);
    if (IS_ERR(dev_class)) {
        pr_err("Could not create device class %s\n", MY_CLASS_NAME);
        goto r_class;
    }

    // Create the device
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
    struct my_node *node1, *tmp1;
    struct my_node *node2, *tmp2;
    struct my_node *node3, *tmp3;

    pr_info("Exiting module: %s\n", KBUILD_MODNAME);
    
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Released major:minor numbers\n");

    list_for_each_entry_safe(node1, tmp1, &ALPHA_head, my_list1)
    {
        pr_info("Deleting ALPHA list node with data = %c\n", node1->data);
        list_del(&node1->my_list1);
    }

    list_for_each_entry_safe(node2, tmp2, &VOWEL_head, my_list)
    {
        pr_info("Deleting VOWEL list node with data = %c\n", node2->data);
        list_del(&node2->my_list);
        kfree(node2);
    }

    list_for_each_entry_safe(node3, tmp3, &CONSO_head, my_list)
    {
        pr_info("Deleting CONSONANT list node with data = %c\n", node3->data);
        list_del(&node3->my_list);
        kfree(node3);
    }
	
    pr_info("Module %s removed successfully\n", KBUILD_MODNAME);
}


module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EDD <edd@cdac.gov.in>");
MODULE_DESCRIPTION("Hello world module!");

