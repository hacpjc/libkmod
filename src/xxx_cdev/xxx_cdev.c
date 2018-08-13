
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/bug.h>
#include <linux/ioctl.h>

#include "xxx_cdev.h"

#define DBG(fmt, args...) printk(KERN_DEBUG "[%s#%d] " fmt "\n", __FUNCTION__, __LINE__, ##args)
#define PRT(fmt, args...) printk(KERN_INFO "[%s#%d] " fmt "\n", __FUNCTION__, __LINE__, ##args)
#define ERR(fmt, args...) do { printk(KERN_ERR "[%s#%d] " fmt "\n", __FUNCTION__, __LINE__, ##args); } while (0)

struct xxx_cdev_entry
{
	unsigned int active;
	struct file_operations *fops;
};

static struct xxx_cdev_entry xxx_cdev_tbl[XXX_CDEV_ENTRY_IDX_MAX];

#define XXX_CDEV_ENTRY(_idx) (&(xxx_cdev_tbl[(_idx)]))

static inline void init_xxx_cdev_entry(struct xxx_cdev_entry *e)
{
	e->active = 0;
	e->fops = NULL;
}

static inline int verify_fops(struct file_operations *fops)
{
	if (fops == NULL)
	{
		ERR("Invalid fops");
		return -1;
	}

	if (fops->owner == NULL)
	{
		ERR("Invalid fops owner");
		return -1;
	}

	return 0; // safe fops
}

/*!
 * \brief Add a new xxx fops at module init. Plz also unregister at module exit.
 */
int xxx_cdev_register_entry(const xxx_cdev_entry_idx_t eidx, struct file_operations *fops)
{
	struct xxx_cdev_entry *e;

	if (((unsigned int) eidx) >= XXX_CDEV_ENTRY_IDX_MAX)
	{
		ERR("Invalid cdev entry idx %u", (unsigned int) eidx);
		return -1;
	}

	e = XXX_CDEV_ENTRY(eidx);

	/*
	 * Verify entry
	 */
	if (e->active)
	{
		ERR("Cannot register cdev entry %u again.", eidx);
		return -1;
	}

	if (verify_fops(fops))
	{
		return -1;
	}

	e->fops = fops;
	e->active = 1;

	return 0;
}

void xxx_cdev_unregister_entry(const xxx_cdev_entry_idx_t eidx, struct file_operations *fops)
{
	struct xxx_cdev_entry *e = XXX_CDEV_ENTRY(eidx);

	if (e->active)
	{
		e->fops = NULL;
		e->active = 0;
	}
	else
	{
		DBG(" * WARNING: Cannot unregister empty cdev entry %u", eidx);
	}
}

int xxx_cdev_init(void)
{
	unsigned int i;

	for (i = 0; i < XXX_CDEV_ENTRY_IDX_MAX; i++)
	{
		init_xxx_cdev_entry(&(xxx_cdev_tbl[i]));
	}

	return 0;
}

void xxx_cdev_exit(void)
{
	unsigned int i;

	for (i = 0; i < XXX_CDEV_ENTRY_IDX_MAX; i++)
	{
		struct xxx_cdev_entry *e = XXX_CDEV_ENTRY(i);

		BUG_ON(e->active);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct xxx_cdev_entry *lookup_xxx_cdev_entry(const struct file *filp)
{
	struct xxx_cdev_entry *e;
	unsigned int maj, min, idx;

	BUG_ON(filp == NULL);

	maj = imajor(file_inode(filp));
	min = iminor(file_inode(filp));
	idx = XXX_CDEV_ENTRY_MINOR2IDX(min);
	if (idx >= XXX_CDEV_ENTRY_IDX_MAX)
	{
		ERR("Invalid cdev (major %u, minor %u) -> idx %u", maj, min, idx);
		return NULL;
	}

	e = XXX_CDEV_ENTRY(idx);
	if (!e->active)
	{
		DBG("cdev entry %u is turned off.", idx);
		return NULL;
	}

	BUG_ON(e->fops == NULL);
	return e;
}

static long chrdev_unlocked_ioctl(
	struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct xxx_cdev_entry *e;

	DBG("Exec");

	e = lookup_xxx_cdev_entry(filp);
	if (e == NULL)
	{
		return -ENODEV;
	}

	return e->fops->unlocked_ioctl ? e->fops->unlocked_ioctl(filp, cmd, arg) : -ENODEV;
}

#if HAVE_COMPAT_IOCTL
static long chrdev_compat_ioctl(struct file *filp
	, unsigned int cmd, unsigned long arg)
{
	struct xxx_cdev_entry *e;

	DBG("Exec");

	e = lookup_xxx_cdev_entry(filp);
	if (e == NULL)
	{
		return -ENODEV;
	}

	return e->fops->compat_ioctl ? e->fops->compat_ioctl(filp, cmd, arg) : -ENODEV;
}
#endif

static int chrdev_open(struct inode *inode, struct file *filp)
{
	struct xxx_cdev_entry *e;

	DBG("Exec");

	e = lookup_xxx_cdev_entry(filp);
	if (e == NULL)
	{
		return -ENODEV;
	}

	return e->fops->open ? e->fops->open(inode, filp) : -EINVAL;
}

static int chrdev_release(struct inode *inode, struct file *filp)
{
	struct xxx_cdev_entry *e;

	DBG("Exec");

	e = lookup_xxx_cdev_entry(filp);
	if (e == NULL)
	{
		return -ENODEV;
	}

	return e->fops->release ? e->fops->release(inode, filp) : -EINVAL;
}

static ssize_t chrdev_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	struct xxx_cdev_entry *e;

	DBG("Exec");

	e = lookup_xxx_cdev_entry(filp);
	if (e == NULL)
	{
		return -ENODEV;
	}

	return e->fops->read ? e->fops->read(filp, buf, len, off) : -EINVAL;
}

static ssize_t chrdev_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	struct xxx_cdev_entry *e;

	DBG("Exec");

	e = lookup_xxx_cdev_entry(filp);
	if (e == NULL)
	{
		return -ENODEV;
	}

	return e->fops->write ? e->fops->write(filp, buf, len, off) : -EINVAL;
}

static unsigned int chrdev_poll(struct file *filp, struct poll_table_struct *pts)
{
	struct xxx_cdev_entry *e;

	DBG("Exec");

	e = lookup_xxx_cdev_entry(filp);
	if (e == NULL)
	{
		return -ENODEV;
	}

	return e->fops->poll ? e->fops->poll(filp, pts) : 0;
}

static int chrdev_flush(struct file *filp, fl_owner_t id)
{
	struct xxx_cdev_entry *e;

	DBG("Exec");

	e = lookup_xxx_cdev_entry(filp);
	if (e == NULL)
	{
		return -ENODEV;
	}

	return e->fops->flush ? e->fops->flush(filp, id) : -EINVAL;
}

static struct file_operations xxx_cdev_fops =
{
	owner: THIS_MODULE,
	unlocked_ioctl: chrdev_unlocked_ioctl,
#if HAVE_COMPAT_IOCTL // <linux/fs.h>
	compat_ioctl: chrdev_compat_ioctl,
#endif
	open:    chrdev_open,
	release: chrdev_release,
	read:    chrdev_read,
	write:   chrdev_write,
	poll:    chrdev_poll,
	flush:   chrdev_flush,
	llseek: no_llseek,
};

#define USE_DEPRECATED_FUNC (0)

#if USE_DEPRECATED_FUNC
int xxx_cdev_register(void)
{
	PRT("Init chrdev /dev/%s with major %d", XXX_CDEV_NAME, XXX_CDEV_MAJ);

	{
		int res;

		res = register_chrdev(XXX_CDEV_MAJ, XXX_CDEV_NAME, &xxx_cdev_fops);
		if (res < 0)
		{
			ERR("Cannot register chrdev with major %d\n", XXX_CDEV_MAJ);
			return -1;
		}
	}

	return 0;
}

void xxx_cdev_unregister(void)
{
	PRT("Exit chrdev /dev/%s with major %d", XXX_CDEV_NAME, XXX_CDEV_MAJ);

	unregister_chrdev(XXX_CDEV_MAJ, XXX_CDEV_NAME);
}

#else

static struct cdev xxx_cdev;

int xxx_cdev_register(void)
{
	PRT("Init chrdev /dev/%s with major %d", XXX_CDEV_NAME, XXX_CDEV_MAJ);

	if (register_chrdev_region(MKDEV(XXX_CDEV_MAJ, XXX_CDEV_MIN), XXX_CDEV_ENTRY_IDX_MAX, XXX_MOD_NAME))
	{
		ERR("Cannot register chrdev region major %u minor %u-%u",
			XXX_CDEV_MAJ,
			XXX_CDEV_ENTRY_IDX2MINOR(0),XXX_CDEV_ENTRY_IDX2MINOR(XXX_CDEV_ENTRY_IDX_MAX));
		return -1;
	}

	cdev_init(&xxx_cdev, &xxx_cdev_fops);

	if (cdev_add(&xxx_cdev, MKDEV(XXX_CDEV_MAJ, XXX_CDEV_MIN), XXX_CDEV_ENTRY_IDX_MAX))
	{
		ERR("Cannot register chrdev with major %u minor %u-%u",
			XXX_CDEV_MAJ,
			XXX_CDEV_ENTRY_IDX2MINOR(0),XXX_CDEV_ENTRY_IDX2MINOR(XXX_CDEV_ENTRY_IDX_MAX));
		goto error_add;
	}

	return 0;

error_add:
	unregister_chrdev_region(MKDEV(XXX_CDEV_MAJ, XXX_CDEV_MIN), XXX_CDEV_ENTRY_IDX_MAX);
	return -1;
}

void xxx_cdev_unregister(void)
{
	PRT("Exit chrdev /dev/%s with major %d", XXX_CDEV_NAME, XXX_CDEV_MAJ);

	cdev_del(&xxx_cdev);

	unregister_chrdev_region(MKDEV(XXX_CDEV_MAJ, XXX_CDEV_MIN), XXX_CDEV_ENTRY_IDX_MAX);
}
#endif

