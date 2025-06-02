#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/tty.h>
#include <linux/tty_ldisc.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/timekeeping.h>

#define DEVICE_NAME     "secdd"
#define BUF_SAMPLES     64
#define ESP_IOCTL_SEL   _IOW('e', 1, int)

struct sample {
    u64 ts;
    int val;
};

static struct {
    struct sample buf[2][BUF_SAMPLES];
    int head[2], tail[2];
    int sel_chan;
    struct mutex lock;
} ring;

static dev_t devt;
static struct cdev cdev;
static struct class *cls;

//
// --- LINE DISCIPLINE CALLBACKS ---
//

static void flush_line(const u8 *line, int len) {
    int s0, s1;
    if (sscanf(line, "%d,%d", &s0, &s1) == 2) {
        u64 now = ktime_get_real_seconds();
        mutex_lock(&ring.lock);
        ring.buf[0][ring.head[0]] = (struct sample){ now, s0 };
        ring.buf[1][ring.head[1]] = (struct sample){ now, s1 };
        pr_info("Guardado: canal0=%d canal1=%d ts=%llu\n", s0, s1, now);
        ring.head[0] = (ring.head[0] + 1) % BUF_SAMPLES;
        ring.head[1] = (ring.head[1] + 1) % BUF_SAMPLES;
        mutex_unlock(&ring.lock);
    }
}

static void esp_receive(struct tty_struct *tty, const u8 *data, const u8 *flags, size_t count) {
    static u8 line[32];
    static int idx = 0;
    size_t i;
    for (i = 0; i < count; i++) {
        u8 c = data[i];
        if (c == '\n' || idx >= sizeof(line) - 1) {
            line[idx] = '\0';
            flush_line(line, idx);
            idx = 0;
        } else {
            line[idx++] = c;
        }
    }
}

static int esp_open(struct tty_struct *tty) {
    return 0;
}

static void esp_close(struct tty_struct *tty) {
}

static struct tty_ldisc_ops esp_ldisc = {
    .owner        = THIS_MODULE,
    .num          = 21,
    .name         = "esp32ldisc",
    .open         = esp_open,
    .close        = esp_close,
    .receive_buf  = esp_receive,
};

//
// --- CHARACTER DEVICE CALLBACKS ---
//

static ssize_t secdd_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    struct sample samp;
    int tail;

    if (len < sizeof(samp)) return -EINVAL;

    mutex_lock(&ring.lock);
    tail = ring.tail[ring.sel_chan];
    if (tail == ring.head[ring.sel_chan]) {
        mutex_unlock(&ring.lock);
        return 0; // buffer vacío
    }
    samp = ring.buf[ring.sel_chan][tail];
    ring.tail[ring.sel_chan] = (tail + 1) % BUF_SAMPLES;
    mutex_unlock(&ring.lock);

    if (copy_to_user(buf, &samp, sizeof(samp)))
        return -EFAULT;
    return sizeof(samp);
}

static long secdd_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int ch = (int)arg;
    if (cmd != ESP_IOCTL_SEL || (ch != 0 && ch != 1))
        return -EINVAL;

    mutex_lock(&ring.lock);
    ring.sel_chan = ch;
    ring.tail[ch] = ring.head[ch]; // reset del buffer al cambiar de canal
    mutex_unlock(&ring.lock);
    return 0;
}

static const struct file_operations secdd_fops = {
    .owner          = THIS_MODULE,
    .read           = secdd_read,
    .unlocked_ioctl = secdd_ioctl,
};

//
// --- INIT / EXIT ---
//

static int __init esp_cdd_init(void) {
    int ret;

    pr_info("esp32_cdd: inicializando...\n");

    mutex_init(&ring.lock);
    ring.head[0] = ring.tail[0] = 0;
    ring.head[1] = ring.tail[1] = 0;
    ring.sel_chan = 0;

    ret = tty_register_ldisc(&esp_ldisc);
    if (ret) {
        pr_err("esp32_cdd: error al registrar ldisc (%d)\n", ret);
        return ret;
    }

    ret = alloc_chrdev_region(&devt, 0, 1, DEVICE_NAME);
    if (ret) {
        tty_unregister_ldisc(&esp_ldisc);
        return ret;
    }

    cdev_init(&cdev, &secdd_fops);
    cdev.owner = THIS_MODULE;
    ret = cdev_add(&cdev, devt, 1);
    if (ret) {
        unregister_chrdev_region(devt, 1);
        tty_unregister_ldisc(&esp_ldisc);
        return ret;
    }

    cls = class_create(DEVICE_NAME); // CORREGIDO para kernel 6.x
    if (IS_ERR(cls)) {
        cdev_del(&cdev);
        unregister_chrdev_region(devt, 1);
        tty_unregister_ldisc(&esp_ldisc);
        return PTR_ERR(cls);
    }

    device_create(cls, NULL, devt, NULL, DEVICE_NAME);
    pr_info("esp32_cdd: listo. LDISC=21 /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit esp_cdd_exit(void) {
    device_destroy(cls, devt);
    class_destroy(cls);
    cdev_del(&cdev);
    unregister_chrdev_region(devt, 1);
    tty_unregister_ldisc(&esp_ldisc);
    pr_info("esp32_cdd: descargado.\n");
}

module_init(esp_cdd_init);
module_exit(esp_cdd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Code Genesis");
MODULE_DESCRIPTION("Driver de carácter para ESP32 - CDD /dev/secdd");

