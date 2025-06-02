//Programa utilizado para leer datos de un canal específico del dispositivo ESP32
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>

#define ESP_IOCTL_SEL _IOW('e', 1, int)

struct sample {
    uint64_t ts;
    int32_t val;
};

int main() {
    int fd = open("/dev/secdd", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // Cambiar canal: 0 o 1
    int canal = 1;
    if (ioctl(fd, ESP_IOCTL_SEL, canal) < 0) {
        perror("ioctl");
        return 1;
    }

    struct sample s;
    while (read(fd, &s, sizeof(s)) == sizeof(s)) {
        printf("Canal %d - t=%llu s, val=%d\n", canal, s.ts, s.val);
        sleep(1);  // una lectura por segundo
    }

    close(fd);
    return 0;
}
