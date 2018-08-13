#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>

#include "xxx_cdev.h"

static int local_kmod_init(void)
{
	if (!xxx_cdev_init())
	{
		return xxx_cdev_register();
	}

	return -EINVAL;
}

static void local_kmod_exit(void)
{
	xxx_cdev_unregister();
	xxx_cdev_exit();
}

MODULE_LICENSE("Dual MIT/GPL");
module_init(local_kmod_init);
module_exit(local_kmod_exit);

