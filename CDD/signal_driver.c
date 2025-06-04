// signal_driver.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/init.h>
#include <linux/version.h>

#define DEVICE_NAME "signal_driver"
#define CLASS_NAME "signal"
#define BUFFER_SIZE 128
#define READ_INTERVAL_MS 200

static int major;
static struct class *signal_class = NULL;
static struct device *signal_device = NULL;

static struct file *serial_filp = NULL;
static struct task_struct *reader_thread = NULL;

static char buffer[BUFFER_SIZE];
static int buffer_index = 0;
static DEFINE_MUTEX(buffer_mutex);

// Prototipos
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static int reader_function(void *data);

// Operaciones del dispositivo
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
};

// Hilo de lectura desde /dev/ttyUSB0
static int reader_function(void *data) {
    loff_t pos = 0;
    char ch;
    int ret;

    serial_filp = filp_open("/dev/ttyUSB0", O_RDONLY, 0);
    if (IS_ERR(serial_filp)) {
        pr_err("No se pudo abrir /dev/ttyUSB0\n");
        return PTR_ERR(serial_filp);
    }

    while (!kthread_should_stop()) {
        ret = kernel_read(serial_filp, &ch, 1, &pos);
        if (ret > 0) {
            mutex_lock(&buffer_mutex);
            if (buffer_index < BUFFER_SIZE - 1) {
                buffer[buffer_index++] = ch;
                buffer[buffer_index] = '\0';
            } else {
                buffer_index = 0;
                buffer[0] = ch;
                buffer[1] = '\0';
            }
            mutex_unlock(&buffer_mutex);
        }

        msleep(READ_INTERVAL_MS);
    }

    filp_close(serial_filp, NULL);
    return 0;
}

static int __init signal_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_alert("Fallo al registrar el dispositivo\n");
        return major;
    }

    signal_class = class_create(CLASS_NAME);
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

    mutex_init(&buffer_mutex);

    reader_thread = kthread_run(reader_function, NULL, "reader_thread");
    if (IS_ERR(reader_thread)) {
        device_destroy(signal_class, MKDEV(major, 0));
        class_destroy(signal_class);
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(reader_thread);
    }

    pr_info("Driver inicializado correctamente\n");
    return 0;
}

static void __exit signal_exit(void) {
    if (reader_thread) {
        kthread_stop(reader_thread);
        reader_thread = NULL;  // Libera referencia
    }

    device_destroy(signal_class, MKDEV(major, 0));
    class_destroy(signal_class);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Driver descargado\n");
}

static int device_open(struct inode *inodep, struct file *filep) {
    return 0;
}

static int device_release(struct inode *inodep, struct file *filep) {
    return 0;
}

static ssize_t device_read(struct file *filep, char __user *user_buffer, size_t len, loff_t *offset) {
    ssize_t bytes_read = 0;

    mutex_lock(&buffer_mutex);
    if (*offset >= buffer_index) {
        mutex_unlock(&buffer_mutex);
        return 0;
    }

    if (len > buffer_index - *offset)
        len = buffer_index - *offset;

    if (copy_to_user(user_buffer, buffer + *offset, len)) {
        mutex_unlock(&buffer_mutex);
        return -EFAULT;
    }

    *offset += len;
    bytes_read = len;
    mutex_unlock(&buffer_mutex);

    return bytes_read;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TP Sistemas de Computación");
MODULE_DESCRIPTION("CDD que lee señales desde ESP32 vía /dev/ttyUSB0");
MODULE_VERSION("0.1");

module_init(signal_init);
module_exit(signal_exit);



