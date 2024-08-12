#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/uaccess.h>
#include <linux/sysfs.h>
#include <linux/debugfs.h>
#include <linux/kallsyms.h>


static unsigned long watch_addr = 0;
module_param(watch_addr, ulong, 0644);
MODULE_PARM_DESC(watch_addr, "Memory address to set watchpoint");

static struct jprobe jp;
static int pre_handler(struct kprobe *p, struct pt_regs *regs)
{
    pr_info("Memory access detected at %p\n", (void *)watch_addr);
    dump_stack();
    return 0;
}

static ssize_t watch_addr_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%lx\n", watch_addr);
}

static ssize_t watch_addr_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    kstrtoul(buf, 0, &watch_addr);
    pr_info("Watchpoint address set to %lx\n", watch_addr);
    return count;
}

static struct kobj_attribute watch_addr_attribute = __ATTR(watch_addr, 0644, watch_addr_show, watch_addr_store);

static struct attribute *attrs[] = {
    &watch_addr_attribute.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *watch_kobj;

static int __init watchpoint_init(void)
{
    int ret;

    watch_kobj = kobject_create_and_add("watchpoint", kernel_kobj);
    if (!watch_kobj)
        return -ENOMEM;

    ret = sysfs_create_group(watch_kobj, &attr_group);
    if (ret)
        kobject_put(watch_kobj);

    jp.kp.addr = (kprobe_opcode_t *)watch_addr;
    jp.pre_handler = pre_handler;
    ret = register_jprobe(&jp);
    if (ret < 0) {
        pr_err("register_jprobe failed, returned %d\n", ret);
        sysfs_remove_group(watch_kobj, &attr_group);
        kobject_put(watch_kobj);
        return ret;
    }

    pr_info("Watchpoint module loaded.\n");
    return 0;
}

static void __exit watchpoint_exit(void)
{
    unregister_jprobe(&jp);
    sysfs_remove_group(watch_kobj, &attr_group);
    kobject_put(watch_kobj);
    pr_info("Watchpoint module unloaded.\n");
}

module_init(watchpoint_init);
module_exit(watchpoint_exit);

