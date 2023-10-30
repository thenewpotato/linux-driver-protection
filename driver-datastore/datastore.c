#include <linux/kernel.h>
#include <linux/module.h>

unsigned long DATA = 0;

int datastore_init(void) {
  printk(KERN_INFO "Datastore initialized!\n");
  return 0;
}

void datastore_exit(void) {
  printk(KERN_INFO "Shutting down datastore.\n");
}

module_init(datastore_init);
module_exit(datastore_exit);

void datastore_add(unsigned long n) {
  DATA += n;
}

unsigned long datastore_get(void) {
  return DATA;
}

EXPORT_SYMBOL(datastore_add);
EXPORT_SYMBOL(datastore_get);

MODULE_AUTHOR("Tiger Wang <tiger.wang@yale.edu>");
MODULE_DESCRIPTION("Simple driver for an in-memory int store");
MODULE_LICENSE("GPL");
