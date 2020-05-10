/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"


#define ENCODER_GPIO_A GPIO_NUM_4
#define ENCODER_GPIO_B GPIO_NUM_5
#define GPIO_INPUT_PIN_SEL  (GPIO_Pin_4 | GPIO_Pin_5)

static int tick_count = 0;

static int a_state = 0;
static int b_state = 0;

void modifyTickCount(bool movedForward) {
    if(movedForward) {
        tick_count++;
    } else {
        tick_count--;
    }
}

void gpio_isr_a_handler(void *arg) {

    int new_a_state = gpio_get_level(ENCODER_GPIO_A);
    if(new_a_state == a_state){
        return;
    }
    modifyTickCount((new_a_state && b_state) || (!new_a_state && !b_state));
    a_state = new_a_state;
}

void gpio_isr_b_handler(void *arg) {

    int new_b_state = gpio_get_level(ENCODER_GPIO_B);
    if(new_b_state == b_state){
        return;
    }
    modifyTickCount((new_b_state && !a_state) || (!new_b_state && a_state));
    b_state = new_b_state;
}

void setup() {
    /* Set the GPIO as a pull input */
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(ENCODER_GPIO_A, gpio_isr_a_handler, (void *) ENCODER_GPIO_A);
    gpio_isr_handler_add(ENCODER_GPIO_B, gpio_isr_b_handler, (void *) ENCODER_GPIO_B);
}

void app_main()
{
    setup();
    while(1) {
        vTaskDelay(100 / portTICK_RATE_MS);
        printf("tick_count %d\n", tick_count);
    }
}
