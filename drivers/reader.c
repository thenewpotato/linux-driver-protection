#include <linux/kernel.h>
#include <linux/module.h>

extern void datastore_add(unsigned long);
extern unsigned long datastore_get(void);

int reader_init(void) {
  printk(KERN_INFO "Reader: Adding 10 to datastore and reading value\n");
  datastore_add(10);
  unsigned long result = datastore_get();
  printk(KERN_INFO "Reader: Value is %lu\n", result);
  return 0;
}

void reader_exit(void) {
  printk(KERN_INFO "Shutting down reader.\n");
}

module_init(reader_init);
module_exit(reader_exit);

MODULE_AUTHOR("Tiger Wang <tiger.wang@yale.edu>");
MODULE_DESCRIPTION("Simple reader of the datastore");
MODULE_LICENSE("GPL");
