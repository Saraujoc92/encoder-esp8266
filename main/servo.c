
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "esp8266/gpio_register.h"
#include "driver/pwm.h"

#include "servo.h"

#define PWM_0_OUT_IO_NUM   12
#define PWM_PERIOD    (20000)
// pwm pin number
const uint32_t pin_num[1] = {
    PWM_0_OUT_IO_NUM
};

// duties table, real_duty = duties[x]/PERIOD
uint32_t duties[1] = {1000};

// phase table, delay = (phase[x]/360)*PERIOD
int16_t phase[1] = {0};

void setupServo() {
    pwm_init(PWM_PERIOD, duties, 1, pin_num);
    pwm_set_phases(phase);
    pwm_start();

}

void moveServo(int32_t angle) {
    //TODO: sanitize this
    printf("got angle %d\n", angle);
    duties[0] = 1000 + angle*1000/180;
    printf("setting pwm %d\n", duties[0]);
    pwm_set_duties(duties);
    pwm_start();
}