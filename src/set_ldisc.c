// Programa que asigna el Line Discipline al puerto del ESP32
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/tty.h>
#include <sys/ioctl.h>

int main() {
    int fd = open("/dev/ttyUSB0", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    int ldisc = 21;
    if (ioctl(fd, TIOCSETD, &ldisc) < 0) {
        perror("ioctl TIOCSETD");
        close(fd);
        return 1;
    }

    printf("Line discipline 21 seteado correctamente.\n");
    close(fd);
    return 0;
}
