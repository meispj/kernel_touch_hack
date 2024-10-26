#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/list.h>
#include <linux/version.h>
#include "Input.h"
#define DEVICE_NAME "PeiJue"
int dispatch_open(struct inode *node, struct file *file)
{
	return 0;
}

int dispatch_close(struct inode *node, struct file *file)
{
	return 0;
}


long dispatch_ioctl(struct file *const file, unsigned int const cmd, unsigned long const arg)
{
    char name[0x100] = {0};
    if (cmd==1&&input_dev!=NULL) 
    {
    UpTouch uptc;
    if (copy_from_user(&uptc, (void __user *)arg, sizeof(uptc)) != 0)
    {
        return -1;
    }
    
    if (uptc.isdown) 
    {
    simulate_touch_event(input_dev,uptc.x,uptc.y);
    return 0;
    }
    else 
    {
    release_simulated_touch(input_dev);
    return 0;
    }
    
    return 0;
    }else if (cmd==-1) {
     if(copy_from_user(name, (void __user*)arg, sizeof(name) - 1) != 0) 
    {
        return -1;
    }
    
    list_for_each_entry(input_dev, input_dev_list, node) 
    {
        
        if(strcmp(input_dev->name,name)==0)
        {
        printk(KERN_INFO "Input_device_name: %s\n", input_dev->name);
        break;
        }
    }
    return 0;
    }

    return 0;
}

struct file_operations dispatch_functions = {
	.owner = THIS_MODULE,
	.open = dispatch_open,
	.release = dispatch_close,
	.unlocked_ioctl = dispatch_ioctl,
};

struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dispatch_functions,
};


static int __init kprobe_init(void) {
    int ret;
    #if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 61))
    register_kprobe(&kp);//4系列不支持kprobe来获取函数指针但是有符号导出
    kallsyms_lookup_name_ptr = (kallsyms_lookup_name_t)kp.addr;
    input_dev_list = (struct list_head *)kallsyms_lookup_name_ptr("input_dev_list");
    #else
    input_dev_list=kallsyms_lookup_name("input_dev_list");
    #endif
    printk("[+] driver_entry");
	ret = misc_register(&misc);
    return ret;
}

static void __exit kprobe_exit(void) {
    unregister_kprobe(&kp);
    printk("[+] driver_unload");
	misc_deregister(&misc);
}

module_init(kprobe_init);
module_exit(kprobe_exit);
MODULE_LICENSE("GPL");