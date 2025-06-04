#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 128

int main() {
    char buffer[128];
    int fd = open("/dev/signal_driver", O_RDONLY);
    if (fd < 0) {
        perror("Error al abrir el dispositivo");
        return 1;
    }

    read(fd, buffer, BUFFER_SIZE); // Leer el buffer completo
    printf("Datos recibidos:\n%s\n", buffer);
    close(fd);

    return 0;
}
