
//ultrasoic
#include <stdio.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ultrasonic.h>
#include <esp_err.h>


#define MAX_DISTANCE_CM 500 // 5m max

#define TRIGGER_GPIO 33
#define ECHO_GPIO 32

//co2
#include <stdio.h>
#include "driver/uart.h"
#include "esp_log.h"

#define TXD_PIN 17
#define RXD_PIN 16
#define UART_NUM UART_NUM_2
#define BUF_SIZE (128)

static const char *TAG = "MH-Z19B";

void co2(void *pvParameters){
    // UART configuration
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    uart_driver_install(UART_NUM_2, BUF_SIZE*2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    uint8_t request[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; // CO₂ measurement request
    uint8_t response[9];

    while (1) {
        uart_write_bytes(UART_NUM, (const char*)request, sizeof(request)); // Send request
        vTaskDelay(pdMS_TO_TICKS(100)); // Wait for response

        int len = uart_read_bytes(UART_NUM, response, sizeof(response), pdMS_TO_TICKS(200));
        if (len == 9 && response[0] == 0xFF && response[1] == 0x86) {
            int co2_ppm = (response[2] << 8) | response[3]; // Extract CO₂ value
            ESP_LOGI(TAG, "CO₂ Concentration: %d ppm", co2_ppm);
        } else {
            ESP_LOGW(TAG, "Invalid response");
        }

        vTaskDelay(pdMS_TO_TICKS(4000)); // Wait 4 seconds before next reading
    }
}


void ultrasonic_test(void *pvParameters)
{
    ultrasonic_sensor_t sensor = {
        .trigger_pin = TRIGGER_GPIO,
        .echo_pin = ECHO_GPIO
    };

    ultrasonic_init(&sensor);

    while (true)
    {
        float distance;
        esp_err_t res = ultrasonic_measure(&sensor, MAX_DISTANCE_CM, &distance);
        if (res != ESP_OK)
        {
            printf("Error %d: ", res);
            switch (res)
            {
                case ESP_ERR_ULTRASONIC_PING:
                    printf("Cannot ping (device is in invalid state)\n");
                    break;
                case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                    printf("Ping timeout (no device found)\n");
                    break;
                case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                    printf("Echo timeout (i.e. distance too big)\n");
                    break;
                default:
                    printf("%s\n", esp_err_to_name(res));
            }
        }
        else
            printf("Distance: %0.04f cm\n", distance*100);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main()
{
    xTaskCreate(ultrasonic_test, "ultrasonic_test", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
    xTaskCreate(co2,"co2",10000,NULL,5,NULL);
}