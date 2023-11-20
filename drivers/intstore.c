#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>

// max Minor devices
#define MAX_DEV 1

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
    unsigned long data = 0;
};

// global storage for device Major number
static int dev_major = 0;

// sysfs class structure
static struct class *intstore_class = NULL;

// array of mychar_device_data for
static struct intstore_device_data intstore_data[MAX_DEV];

int intstore_init(void) {
    int err, i;
    dev_t dev;

    // allocate chardev region and assign Major number
    err = alloc_chrdev_region(&dev, 0, 1, "intstore");

    dev_major = MAJOR(dev);

    // create sysfs class
    intstore_class = class_create(THIS_MODULE, "intstore");

    // Create necessary number of the devices
    for (i = 0; i < MAX_DEV; i++) {
        // init new device
        cdev_init(&intstore_data[i].cdev, &intstore_fops);
        intstore_data[i].cdev.owner = THIS_MODULE;

        // add device to the system where "i" is a Minor number of the new device
        cdev_add(&intstore_data[i].cdev, MKDEV(dev_major, i), 1);

        // create device node /dev/mychardev-x where "x" is "i", equal to the Minor number
        device_create(intstore_class, NULL, MKDEV(dev_major, i), NULL, "intstore-%d", i);
    }


    printk(KERN_INFO "Intstore initialized!\n");
    return 0;
}

void intstore_exit(void) {
    printk(KERN_INFO "Shutting down intstore.\n");
}

module_init(intstore_init);
module_exit(intstore_exit);

MODULE_AUTHOR("Tiger Wang <tiger.wang@yale.edu>");
MODULE_DESCRIPTION("Simple driver for an in-memory int store");
MODULE_LICENSE("GPL");
