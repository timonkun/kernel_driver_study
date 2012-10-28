/* A simple test case for char driver operation */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>

MODULE_LICENSE("Dual BSD/GPL");

#define SAMPLE2_MAJOR 254
#define SAMPLE2_MINOR 1
#define DEVICE_NAME "sample2_cdev"
#define BUF_LEN 80
#define SUCCESS 0

static struct cdev *sample2_cdev;
static struct class *sample2_class;
dev_t devid;
static char msg[BUF_LEN];
static char *msg_ptr;
static int major;
static int Device_Open=0;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,
};

static int device_open(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk("%s(%p)\n", __func__, file);
#endif
	if(Device_Open)
	  return -EBUSY;

	Device_Open++;
	return SUCCESS; 
}

static int device_release(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk("%s(%p, %p)\n", __func__, inode, file);
#endif
	Device_Open--;
	return SUCCESS;
}

static ssize_t device_read(struct file *file, char *buf, size_t count, loff_t *f_pos)
{
	int i, err;

	if(access_ok(VERIFY_WRITE, buf, count) == -EFAULT)
	    return -EFAULT;
	
	if(BUF_LEN < count)
	{
		printk("%s: only support %d character.\n", __func__, BUF_LEN);
		return -EFAULT;
	}

	msg_ptr = &msg[0];

	err = copy_to_user(buf, msg_ptr, count);
	if(err)
	{
		printk(KERN_ALERT "%s: copy_to_user error.\n", __func__);
		return -EFAULT;
	}
	return count;
}

static ssize_t device_write(struct file *file, const char *buf, size_t count, loff_t *f_pos)
{
	int err;

	if(BUF_LEN < count)
	{
		printk("%s: only support %d character.\n", __func__, BUF_LEN);
		return -EFAULT;
	}

	msg_ptr = &msg[0];
	err = copy_from_user(msg_ptr, buf, count);
	if(err)
	{
		printk(KERN_ALERT "%s: copy_from_user error.\n", __func__);
		return -EFAULT;
	}

	return count;
}

static int sample2_init(void)
{
	int err;
	
	alloc_chrdev_region(&devid, 0, 1, DEVICE_NAME);
	major = MAJOR(devid);

	sample2_cdev = cdev_alloc();
	cdev_init(sample2_cdev, &fops);
	sample2_cdev->owner = THIS_MODULE;
	
	err = cdev_add(sample2_cdev, devid, 1);
	if(err)
	{
		printk(KERN_ALERT "[%s] %s assigned major number %d. err=%d\n",
				__FILE__, __func__, major, err);
		return -1;
	}
	
	 /* create your own class under /sysfs */
	sample2_class = class_create(THIS_MODULE, "sample2_class");
	if(IS_ERR(sample2_class))
	{
		printk(KERN_ALERT "[%s] %s create class error.\n",
					__FILE__, __func__);
		return -1;
	}

	 /* register your own device in sysfs, and this will cause udev to create corresponding device node */
	device_create(sample2_class, NULL, devid, NULL, DEVICE_NAME"%d", 0);

    printk(KERN_ALERT "[%s] %s major number is %d.\n",
				__FILE__, __func__, MAJOR(devid));
    return 0;
}

static void sample2_exit(void)
{
	cdev_del(sample2_cdev);
	device_destroy(sample2_class, devid);
	class_destroy(sample2_class);
	printk(KERN_ALERT "[%s] %s clean up done.\n", __FILE__, __func__);
}

module_init(sample2_init);
module_exit(sample2_exit);
