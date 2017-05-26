#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define CHARDEV_BUFFLEN		1024
static unsigned char chardev_buff[CHARDEV_BUFFLEN];

static int chardev_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "device is open.\n");
	return 0;
}

static int chardev_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "device released.\n");
	return 0;
}

static ssize_t chardev_read(struct file *filp, char __user *buff,
			size_t count, loff_t *offp)
{
	printk(KERN_INFO "read from device.\n");
	return count;
}

static ssize_t chardev_write(struct file *filp, const char __user *buff,
			size_t count, loff_t *offp)
{
	unsigned long remain;

	printk(KERN_INFO "write to device.\n");
	if (count > CHARDEV_BUFFLEN)
		return -EINVAL;

	remain = copy_from_user(chardev_buff, buff, count);
	if (remain > 0)
		printk(KERN_INFO "%ld bytes remain in user buffer.\n",
		       remain);
	chardev_buff[count - remain] = '\0';
	printk(KERN_INFO "received string = %s.\n", chardev_buff);

	return count - remain;
}

static struct file_operations chardev_fops = {
	.owner = THIS_MODULE,
	.open = chardev_open,
	.read = chardev_read,
	.write = chardev_write,
	.release = chardev_release,
};

static struct cdev *cdev;
static const dev_t dev = MKDEV(240, 0);

static int __init hello_init(void)
{
	int err;

	err = register_chrdev_region(dev, 1, "hello");
	if (err) {
		printk(KERN_ERR "register_chrdev_region failed.\n");
		return -1;
	}

	cdev = cdev_alloc();
	if (!cdev) {
		printk(KERN_ERR "cdev_alloc failed.\n");
		return -1;
	}

	cdev->ops = &chardev_fops;
	err = cdev_add(cdev, dev, 1);
	if (err) {
		printk(KERN_ERR "cdev_add failed.\n");
		return -1;
	}

	printk(KERN_INFO "hello device is ready for I/O.\n");

	return err;
}

static void __exit hello_exit(void)
{
	if (cdev)
		cdev_del(cdev);
	unregister_chrdev_region(dev, 1);

	printk(KERN_INFO "hello device removed.\n");
	return;
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Katsuya Matsubara");
