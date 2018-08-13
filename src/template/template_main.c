#include <linux/init.h>
#include <linux/module.h>

static int local_kmod_init(void)
{
	printk(KERN_INFO "init\n");
	return 0;
}

static void local_kmod_exit(void)
{
	printk(KERN_INFO "exit\n");
}

MODULE_LICENSE("Dual MIT/GPL");
module_init(local_kmod_init);
module_exit(local_kmod_exit);

