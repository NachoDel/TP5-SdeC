#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/uart.h"
#include "esp_log.h"

#define TXD_PIN (GPIO_NUM_1)
#define UART_PORT UART_NUM_0
#define BUF_SIZE (1024)

static int signal0 = 0;
static int signal1 = 0;
static int count = 0;

void app_main(void)
{
    // Configuración UART (UART0 ya está conectado al USB en la mayoría de devkits)
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_PORT, &uart_config);
    uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0);

    while (1) {

        // Simular señales
        count = count + 1;
        if (count == 10)
        {
            signal0 = 10;
        } else if (count == 20)
        {
            signal0 = 0;
            signal1 = 0;
            count = 0;
        }

        signal1 = signal1 + 1;

        // Formatear string
        char msg[32];
        int len = snprintf(msg, sizeof(msg), "%d,%d\n", signal0, signal1);

        // Enviar por UART
        uart_write_bytes(UART_PORT, msg, len);

        // Log por consola (opcional)
        // ESP_LOGI("ESP32", "Sent: %s", msg);

        // Esperar 100 ms
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}