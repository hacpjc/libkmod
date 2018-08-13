#ifndef XXX_CDEV_H_
#define XXX_CDEV_H_

#include <linux/fs.h>

typedef enum
{
	XXX_CDEV_ENTRY_IDX_DFL = 0,
	XXX_CDEV_ENTRY_IDX_MAX
} xxx_cdev_entry_idx_t;

#define XXX_MOD_NAME "xxx"
#define XXX_CDEV_NAME "xxx"
#define XXX_CDEV_MAJ (191)
#define XXX_CDEV_MIN (0)

#define XXX_CDEV_ENTRY_IDX2MINOR(_idx) ((_idx) + XXX_CDEV_MIN)
#define XXX_CDEV_ENTRY_MINOR2IDX(_minor) ((_minor) - XXX_CDEV_MIN)

/*
 * Initialize/exit cdev
 */
int xxx_cdev_init(void);
void xxx_cdev_exit(void);

/*
 * Register cdev to kernel and be ready to receive calls.
 */
int xxx_cdev_register(void);
void xxx_cdev_unregister(void);

/*
 * Register your cdev fops with a corresponding index value. Cannot register twice.
 */
int xxx_cdev_register_entry(const xxx_cdev_entry_idx_t eidx, struct file_operations *fops);
void xxx_cdev_unregister_entry(const xxx_cdev_entry_idx_t eidx, struct file_operations *fops);

#endif /* XXX_CDEV_H_ */
