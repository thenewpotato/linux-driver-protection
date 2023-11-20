#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>

// max Minor devices
#define MAX_DEV 1

static int intstore_open(struct inode *inode, struct file *file)
{
    printk("INTSTORE: Device open\n");
    return 0;
}

static int intstore_release(struct inode *inode, struct file *file)
{
    printk("INTSTORE: Device close\n");
    return 0;
}

static long intstore_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    printk("INTSTORE: Device ioctl\n");
    return 0;
}

static ssize_t intstore_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    printk("INTSTORE: Device read\n");
    return 0;
}

static ssize_t intstore_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    printk("INTSTORE: Device write\n");
    return 0;
}

// initialize file_operations
static const struct file_operations intstore_fops = {
    .owner      = THIS_MODULE,
    .open       = intstore_open,
    .release    = intstore_release,
    .unlocked_ioctl = intstore_ioctl,
    .read       = intstore_read,
    .write       = intstore_write
};

// device data holder, this structure may be extended to hold additional data
struct intstore_device_data {
    struct cdev cdev;
    unsigned long data;
};

// global storage for device Major number
static int dev_major = 0;

// sysfs class structure
static struct class *intstore_class = NULL;

// array of mychar_device_data for
static struct intstore_device_data intstore_data[MAX_DEV];

/* Callback to configure character device permissions */
static int intstore_uevent(struct device *dev, struct kobj_uevent_env *env) {
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

int intstore_init(void) {
    int err, i;
    dev_t dev;

    // allocate chardev region and assign Major number
    err = alloc_chrdev_region(&dev, 0, 1, "intstore");

    dev_major = MAJOR(dev);

    // create sysfs class
    intstore_class = class_create(THIS_MODULE, "intstore");
    intstore_class->dev_uevent = intstore_uevent;

    // Create necessary number of the devices
    for (i = 0; i < MAX_DEV; i++) {
        // init new device
        cdev_init(&intstore_data[i].cdev, &intstore_fops);
        intstore_data[i].cdev.owner = THIS_MODULE;
        intstore_data[i].data = 558;

        // add device to the system where "i" is a Minor number of the new device
        cdev_add(&intstore_data[i].cdev, MKDEV(dev_major, i), 1);

        // create device node /dev/intstore-x where "x" is "i", equal to the Minor number
        device_create(intstore_class, NULL, MKDEV(dev_major, i), NULL, "intstore-%d", i);
    }

    printk(KERN_INFO "Intstore initialized!\n");
    return 0;
}

void intstore_exit(void) {
    int i;

    for (i = 0; i < MAX_DEV; i++) {
        device_destroy(intstore_class, MKDEV(dev_major, i));
    }

    class_unregister(intstore_class);
    class_destroy(intstore_class);

    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);

    printk(KERN_INFO "Intstore destroyed.\n");
}

module_init(intstore_init);
module_exit(intstore_exit);

MODULE_AUTHOR("Tiger Wang <tiger.wang@yale.edu>");
MODULE_DESCRIPTION("Simple driver for an in-memory int store");
MODULE_LICENSE("GPL");
