#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

#include <asm/current.h>
#include <asm/system.h>

#define WQ_MINOR 201

static int wq_open(struct inode *inode, struct file *file);
static int wq_release(struct inode *inode, struct file *file);
static ssize_t wq_read(struct file *file, char __user *buf,
				size_t count, loff_t *ppos);
static void wq_timer(unsigned long data);

static int cond = 0;

static DECLARE_WAIT_QUEUE_HEAD(wq_wait);
static DEFINE_TIMER(wq_head, wq_timer, 0, 0);

static const struct file_operations wq_fops = {
	.owner   = THIS_MODULE,
	.read    = wq_read,
	.open    = wq_open,
	.release = wq_release,
};

static struct miscdevice wq_dev = {
	.minor = WQ_MINOR,
	.name  = "wq",
	.fops  = &wq_fops,
};

static void wq_timer(unsigned long data)
{
	pr_info("%s: enter\n", __func__);
	cond = 1;
	wake_up_interruptible(&wq_wait);
}

static int wq_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int wq_release(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t wq_read(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	size_t data;
	ssize_t retval;
	int delay = 2000; /* 2 secs */

	pr_info("%s: enter\n", __func__);

	mod_timer(&wq_head, jiffies + msecs_to_jiffies(delay));

	wait_event_interruptible(wq_wait, (cond == 1));

	cond = 0;
	data = 0x100;

	retval = put_user(data, (size_t __user *)buf);

	return retval;
}

static int __init wq_init(void)
{
	pr_info("%s: enter\n", __func__);

	if (misc_register(&wq_dev)) {
		pr_err("%s: can't register device\n", __func__);
		return -ENODEV;
	}

	return 0;
}

static void __exit wq_exit(void)
{
	pr_info("%s: enter\n", __func__);

	misc_deregister(&wq_dev);
}

module_init(wq_init);
module_exit(wq_exit);

MODULE_LICENSE("GPL");
