#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>

// max Minor devices
#define MAX_DEV 1

#define STRING_LEN 10

// device data holder, this structure may be extended to hold additional data
struct stringstore_device_data {
    struct cdev cdev;
    int string[STRING_LEN];
};

// global storage for device Major number
static int dev_major = 0;

// sysfs class structure
static struct class *stringstore_class = NULL;

// array of mychar_device_data for
static struct stringstore_device_data stringstore_data[MAX_DEV];

static int stringstore_open(struct inode *inode, struct file *file)
{
    printk("stringstore: Device open\n");
    return 0;
}

static int stringstore_release(struct inode *inode, struct file *file)
{
    printk("stringstore: Device close\n");
    return 0;
}

static long stringstore_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    printk("stringstore: Device ioctl\n");
    return 0;
}

static ssize_t stringstore_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    if (count > STRING_LEN) {
        count = STRING_LEN;
    }

    /* Switch to new page table */
    unsigned long old_cr3 = read_cr3_pa();
    unsigned long new_cr3 = __sme_pa(THIS_MODULE->pgd_shadow);
    write_cr3(new_cr3);

    if (copy_to_user(buf, stringstore_data[0].string, count)) {
        return -EFAULT;
    }

    /* Switch back to kernel page table */
    write_cr3(old_cr3);

    return count;
}

static ssize_t stringstore_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    if (count > STRING_LEN - 1) {
        count = STRING_LEN - 1;
    }

    if (copy_from_user(stringstore_data[0].string, buf, count)) {
        return -EFAULT;
    }

    stringstore_data[0].string[count] = '\0';

    return count;
}

// initialize file_operations
static const struct file_operations stringstore_fops = {
    .owner      = THIS_MODULE,
    .open       = stringstore_open,
    .release    = stringstore_release,
    .unlocked_ioctl = stringstore_ioctl,
    .read       = stringstore_read,
    .write       = stringstore_write
};

/* Callback to configure character device permissions */
static int stringstore_uevent(struct device *dev, struct kobj_uevent_env *env) {
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

int stringstore_init(void) {
    int err, i, ret;
    dev_t dev;
    struct device* device;

    // allocate chardev region and assign Major number
    err = alloc_chrdev_region(&dev, 0, 1, "stringstore");
    printk(KERN_INFO "alloc_chrdev_region returns %d\n", err);

    dev_major = MAJOR(dev);

    // create sysfs class
    stringstore_class = class_create(THIS_MODULE, "stringstore");
    stringstore_class->dev_uevent = stringstore_uevent;

    // Create necessary number of the devices
    for (i = 0; i < MAX_DEV; i++) {
        // init new device
        cdev_init(&stringstore_data[i].cdev, &stringstore_fops);
        stringstore_data[i].cdev.owner = THIS_MODULE;
        stringstore_data[i].string[0] = 'H';
        stringstore_data[i].string[1] = 'i';
        stringstore_data[i].string[2] = '\n';
        stringstore_data[i].string[3] = '\0';

        // add device to the system where "i" is a Minor number of the new device
        ret = cdev_add(&stringstore_data[i].cdev, MKDEV(dev_major, i), 1);
        printk(KERN_INFO "cdev_add returns %d\n", ret);

        // create device node /dev/stringstore-x where "x" is "i", equal to the Minor number
        device = device_create(stringstore_class, NULL, MKDEV(dev_major, i), NULL, "stringstore-%d", i);
        if (IS_ERR(device)) {
            printk(KERN_INFO "device_create errored %d\n", PTR_ERR(device));
        }

        printk(KERN_INFO "device initialized with major %d and minor %d\n", dev_major, i);
    }

    printk(KERN_INFO "stringstore initialized!\n");
    return 0;
}

void stringstore_exit(void) {
    int i;

    for (i = 0; i < MAX_DEV; i++) {
        device_destroy(stringstore_class, MKDEV(dev_major, i));
    }

    class_unregister(stringstore_class);
    class_destroy(stringstore_class);

    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);

    printk(KERN_INFO "stringstore destroyed.\n");
}

module_init(stringstore_init);
module_exit(stringstore_exit);

MODULE_AUTHOR("Tiger Wang <tiger.wang@yale.edu>");
MODULE_DESCRIPTION("Simple driver for an in-memory int store");
MODULE_LICENSE("GPL");
