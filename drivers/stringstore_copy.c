#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>

// max Minor devices
#define MAX_DEV 1

#define STRING_LEN 10

// device data holder, this structure may be extended to hold additional data
struct stringstore_copy_device_data {
    struct cdev cdev;
    int string[STRING_LEN];
};

// global storage for device Major number
static int dev_major = 0;

// sysfs class structure
static struct class *stringstore_copy_class = NULL;

// array of mychar_device_data for
static struct stringstore_copy_device_data stringstore_copy_data[MAX_DEV];

static int stringstore_copy_open(struct inode *inode, struct file *file)
{
    printk("stringstore_copy: Device open\n");
    return 0;
}

static int stringstore_copy_release(struct inode *inode, struct file *file)
{
    printk("stringstore_copy: Device close\n");
    return 0;
}

static long stringstore_copy_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    printk("stringstore_copy: Device ioctl\n");
    return 0;
}

static ssize_t stringstore_copy_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    if (count > STRING_LEN) {
        count = STRING_LEN;
    }

    if (copy_to_user(buf, stringstore_copy_data[0].string, count)) {
        return -EFAULT;
    }

    return count;
}

static ssize_t stringstore_copy_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    if (count > STRING_LEN - 1) {
        count = STRING_LEN - 1;
    }

    if (copy_from_user(stringstore_copy_data[0].string, buf, count)) {
        return -EFAULT;
    }

    stringstore_copy_data[0].string[count] = '\0';

    return count;
}

// initialize file_operations
static const struct file_operations stringstore_copy_fops = {
    .owner      = THIS_MODULE,
    .open       = stringstore_copy_open,
    .release    = stringstore_copy_release,
    .unlocked_ioctl = stringstore_copy_ioctl,
    .read       = stringstore_copy_read,
    .write       = stringstore_copy_write
};

/* Callback to configure character device permissions */
static int stringstore_copy_uevent(struct device *dev, struct kobj_uevent_env *env) {
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

int stringstore_copy_init(void) {
    int err, i, ret;
    dev_t dev;
    struct device* device;

    // allocate chardev region and assign Major number
    err = alloc_chrdev_region(&dev, 0, 1, "stringstore_copy");
    printk(KERN_INFO "alloc_chrdev_region returns %d\n", err);

    dev_major = MAJOR(dev);

    // create sysfs class
    stringstore_copy_class = class_create(THIS_MODULE, "stringstore_copy");
    stringstore_copy_class->dev_uevent = stringstore_copy_uevent;

    // Create necessary number of the devices
    for (i = 0; i < MAX_DEV; i++) {
        // init new device
        cdev_init(&stringstore_copy_data[i].cdev, &stringstore_copy_fops);
        stringstore_copy_data[i].cdev.owner = THIS_MODULE;
        stringstore_copy_data[i].string[0] = 'H';
        stringstore_copy_data[i].string[1] = 'i';
        stringstore_copy_data[i].string[2] = '\n';
        stringstore_copy_data[i].string[3] = '\0';

        // add device to the system where "i" is a Minor number of the new device
        ret = cdev_add(&stringstore_copy_data[i].cdev, MKDEV(dev_major, i), 1);
        printk(KERN_INFO "cdev_add returns %d\n", ret);

        // create device node /dev/stringstore_copy-x where "x" is "i", equal to the Minor number
        device = device_create(stringstore_copy_class, NULL, MKDEV(dev_major, i), NULL, "stringstore_copy-%d", i);
        if (IS_ERR(device)) {
            printk(KERN_INFO "device_create errored %d\n", PTR_ERR(device));
        }

        printk(KERN_INFO "device initialized with major %d and minor %d\n", dev_major, i);
    }

    printk(KERN_INFO "stringstore_copy initialized!\n");
    return 0;
}

void stringstore_copy_exit(void) {
    int i;

    for (i = 0; i < MAX_DEV; i++) {
        device_destroy(stringstore_copy_class, MKDEV(dev_major, i));
    }

    class_unregister(stringstore_copy_class);
    class_destroy(stringstore_copy_class);

    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);

    printk(KERN_INFO "stringstore_copy destroyed.\n");
}

module_init(stringstore_copy_init);
module_exit(stringstore_copy_exit);

MODULE_AUTHOR("Tiger Wang <tiger.wang@yale.edu>");
MODULE_DESCRIPTION("Simple driver for an in-memory int store");
MODULE_LICENSE("GPL");
