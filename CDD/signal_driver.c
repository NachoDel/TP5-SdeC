// signal_driver.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/time.h>

#define DEVICE_NAME "signal_driver"

static int major;
static struct task_struct *reader_thread;
static int selected_signal = 0;
static int signal0 = 0;
static int signal1 = 0;
static struct mutex signal_lock;

// Estructura del dispositivo
static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
};

// Hilo lector desde /dev/ttyUSB0
static int reader_function(void *data)
{
    struct file *file;
    char buf[128];
    mm_segment_t oldfs;
    int len;
    char *comma;
    int s0, s1;
    struct timespec64 last_update, now;

    ktime_get_real_ts64(&last_update);

    // Permitir acceso a espacio de usuario
    oldfs = get_fs();
    set_fs(KERNEL_DS);

    // Abrir archivo
    file = filp_open("/dev/ttyUSB0", O_RDONLY, 0);
    if (IS_ERR(file)) {
        printk(KERN_ERR "signal_driver: Error al abrir /dev/ttyUSB0\n");
        set_fs(oldfs);
        return -1;
    }

    while (!kthread_should_stop()) {
        memset(buf, 0, sizeof(buf));
        len = kernel_read(file, buf, sizeof(buf) - 1, &file->f_pos);
        if (len > 0) {
            comma = strchr(buf, ',');
            if (comma) {
                *comma = '\0';
                s0 = simple_strtol(buf, NULL, 10);
                s1 = simple_strtol(comma + 1, NULL, 10);

                ktime_get_real_ts64(&now);
                if (now.tv_sec != last_update.tv_sec) {
                    mutex_lock(&signal_lock);
                    signal0 = s0;
                    signal1 = s1;
                    mutex_unlock(&signal_lock);
                    last_update = now;
                }
            }
        }
        msleep(200);
    }

    filp_close(file, NULL);
    set_fs(oldfs);
    return 0;
}

// Función de lectura
static ssize_t device_read(struct file *filp, char __user *buffer, size_t len, loff_t *offset)
{
    char msg[32];
    int value, ret;

    mutex_lock(&signal_lock);
    value = (selected_signal == 0) ? signal0 : signal1;
    mutex_unlock(&signal_lock);

    snprintf(msg, sizeof(msg), "%d\n", value);

    ret = copy_to_user(buffer, msg, strlen(msg));
    return ret ? -EFAULT : strlen(msg);
}

// Función de escritura
static ssize_t device_write(struct file *filp, const char __user *buffer, size_t len, loff_t *offset)
{
    char kbuf[2];

    if (len < 1)
        return -EINVAL;

    if (copy_from_user(kbuf, buffer, 1))
        return -EFAULT;

    kbuf[1] = '\0';

    if (kbuf[0] == '0' || kbuf[0] == '1') {
        selected_signal = kbuf[0] - '0';
    }

    return len;
}

// Inicialización
static int __init signal_driver_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", major);
        return major;
    }

    mutex_init(&signal_lock);

    reader_thread = kthread_run(reader_function, NULL, "reader_thread");
    if (IS_ERR(reader_thread)) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create reader thread\n");
        return PTR_ERR(reader_thread);
    }

    printk(KERN_INFO "signal_driver loaded: major %d\n", major);
    return 0;
}

// Limpieza
static void __exit signal_driver_exit(void)
{
    if (reader_thread)
        kthread_stop(reader_thread);

    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "signal_driver unloaded\n");
}

module_init(signal_driver_init);
module_exit(signal_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CodeGenesis");
MODULE_DESCRIPTION("Driver de carácter para lectura de señales desde ESP32");

