#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>

static int timer = 0;
static struct timer_list my_timer;

void my_timer_callback(unsigned long data)
{
	printk("Hello, World!\n");
	my_timer.expires += msecs_to_jiffies(timer * 1000);
	add_timer(&my_timer);
}

static ssize_t timer_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "Timer: %d\n", timer);
}

static ssize_t timer_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	sscanf(buf, "%du", &timer);

	if (timer_pending(&my_timer)) {
		int retval = del_timer(&my_timer);
		if (retval)
			printk("The timer is still in use...\n");
	}

	if (timer > 0) {
		my_timer.function = my_timer_callback;
		my_timer.expires = jiffies + msecs_to_jiffies(timer * 1000);
		add_timer(&my_timer);
	}

	return count;
}

static struct kobj_attribute timer_attribute = __ATTR(timer, 0665, timer_show, timer_store);

static struct attribute *attrs[] = {
	&timer_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *timer_kobj;

static int __init timer_init(void)
{
	int retval;

	printk(KERN_INFO "Module initializing...\n");

	timer_kobj = kobject_create_and_add("dev_timer", kernel_kobj);
	if (!timer_kobj)
		return -ENOMEM;

	retval = sysfs_create_group(timer_kobj, &attr_group);
	if (retval)
		kobject_put(timer_kobj);

	init_timer(&my_timer);

	return retval;
}

static void __exit timer_exit(void)
{
	printk(KERN_INFO "Module releasing...\n");

	if (timer_pending(&my_timer)) {
		int retval = del_timer(&my_timer);
		if (retval)
			printk("The timer is still in use...\n");
	}

	kobject_put(timer_kobj);
}

module_init(timer_init);
module_exit(timer_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("bsu");
