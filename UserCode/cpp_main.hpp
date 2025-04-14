#ifndef CPPMAIN_HPP_
#define CPPMAIN_HPP_

#include "main.h"
#include "gpio.h"
#include "tim.h"
#include "can.h"
#include "usart.h"
#include "i2c.h"

//#include "usb_lib.hpp"
#include "uart_lib.hpp"
#include "timer_interruption.hpp"
#include "can_lib.hpp"
#include "led_control.hpp"
//#include "pwm_control.hpp"
//#include "pid_control.hpp"
#include "encoder_lib.hpp"

namespace UserCode
{
    void cpp_main_init(void);
    void cpp_main_while(void);

    void timer_interruption_1kHz();

    void as5600_gpio_init();
    size_t id_read();

    stmlib_v1::stlink_debag pc(&huart2, 10);
    stmlib_v1::led_control led(LED_GPIO_Port, LED_Pin);
    stmlib_v1::can_comm_it master_can(&hcan);
    stmlib_v1::HardTimer hard_timer_1kHz(&htim6);
    stmlib_v1::as5600 as5600_i2c(&hi2c1, true);

    uint8_t board_id;

}

#endif /* CPPMAIN_HPP_ */