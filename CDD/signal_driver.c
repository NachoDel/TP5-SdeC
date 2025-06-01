// signal_driver.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/fdtable.h>

#define DEVICE_NAME "signal_driver"
#define CLASS_NAME  "signal"

static int major;
static struct class*  signal_class  = NULL;
static struct device* signal_device = NULL;

static DEFINE_MUTEX(signal_mutex);

// Señales simuladas
static int selected_signal = 0; // 0 o 1
static int signal0 = 0;
static int signal1 = 0;

// Prototipos
static int     dev_open(struct inode*, struct file*);
static int     dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char __user*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char __user*, size_t, loff_t*);

// File operations
static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

// Funciones básicas
static int dev_open(struct inode *inodep, struct file *filep) {
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    char msg[32];
    int val = selected_signal == 0 ? signal0 : signal1;
    int msg_len = snprintf(msg, sizeof(msg), "%d\n", val);
    return copy_to_user(buffer, msg, msg_len) ? -EFAULT : msg_len;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    char input;
    if (copy_from_user(&input, buffer, 1)) return -EFAULT;
    if (input == '0') selected_signal = 0;
    else if (input == '1') selected_signal = 1;
    else return -EINVAL;
    return len;
}

// Init y exit
static int __init signal_driver_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_alert("Failed to register char device\n");
        return major;
    }

    signal_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(signal_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(signal_class);
    }

    signal_device = device_create(signal_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(signal_device)) {
        class_destroy(signal_class);
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(signal_device);
    }

    mutex_init(&signal_mutex);

    pr_info("signal_driver loaded: major %d\n", major);
    return 0;
}

static void __exit signal_driver_exit(void) {
    device_destroy(signal_class, MKDEV(major, 0));
    class_destroy(signal_class);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("signal_driver unloaded\n");
}

module_init(signal_driver_init);
module_exit(signal_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tu Nombre");
MODULE_DESCRIPTION("Driver CDD que selecciona y entrega señales desde el ESP32");
