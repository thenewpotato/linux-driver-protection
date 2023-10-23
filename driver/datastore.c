#include <linux/kernel.h>
#include <linux/module.h>

MODULE_AUTHOR("Tiger Wang");
MODULE_DESCRIPTION("Simple driver for an in-memory int store");
MODULE_LICENSE("GPL");

int datastore_init(void) {
  printk(KERN_INFO "Hello World!\n");
  return 0;
}

void datastore_exit(void) {
  printk(KERN_INFO "Bye World!\n");
}

module_init(datastore_init);
module_exit(datastore_exit);
