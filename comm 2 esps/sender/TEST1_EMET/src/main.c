#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <string.h>

#define SENDER_TXD_PIN (26)
#define SENDER_RXD_PIN (27)
#define UART_PORT_NUM UART_NUM_2
#define BUF_SIZE 1024

static const char *TAG = "uart_sender";

void app_main(void)
{
    // Configure UART parameters
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    
    // Install UART driver
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUF_SIZE, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, SENDER_TXD_PIN, SENDER_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Replace this with your actual sensor reading code
    float sensor_value = 0.0;
    
    while (1) {
        // Read sensor value (simulated here)
        sensor_value += 0.5;
        if (sensor_value > 100.0) {
            sensor_value = 0.0;
        }
        
        // Format data packet
        char data_packet[50];
        sprintf(data_packet, "SENSOR:%.2f\n", sensor_value);
        
        // Send data
        uart_write_bytes(UART_PORT_NUM, data_packet, strlen(data_packet));
        ESP_LOGI(TAG, "Sent: %s", data_packet);
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Send every second
    }
}