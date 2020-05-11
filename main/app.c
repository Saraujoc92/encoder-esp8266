


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/uart.h"
#include "servo.h"

/**
 * This is an example which echos any data it receives on UART0 back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: UART0
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 */

#define BUF_SIZE (1024)

void parseCommand(char input[]) {

    char* command = strtok(input, " ");
    if(!strcmp("ser", command)) {
        char* args = strtok(NULL, " ");
        printf("command: %s\nargs: %s\n", command, args);
        moveServo(atoi(args));

    } else {
        printf("invalid command\n");
    }
}


static void readCommand() {
    // Configure parameters of an UART driver,
    // communication pins and install the driver
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);

    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    char command[50] = {}; 
    int commandLength = 0;

    // Configure a temporary buffer for the incoming data
    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_0, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        if (len > 0) {
            if ((data[0] == '\r') || (data[0] == '\n')) {
                parseCommand(command);
                memset(command, '\0', sizeof(command));
                commandLength = 0;
            } else {
                for (int i = 0; i <len; i++) {
                    command[commandLength+i] = ((char*) data)[i];
                }
                commandLength += len;
            }
        }
    }
}

void app_main() {
    setupServo();
    xTaskCreate(readCommand, "readCommand", 1024, NULL, 10, NULL);
}