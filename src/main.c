/*
 * main.c
 */
#include "common.h"
#include "I2C_MASTER.h"
#include "clock.h"
#include "calc.h"
#include "lcd.h"
#include "keypad.h"
#include "button.h"
#include "timer.h"
#include "uart0.h"

FILE OUTPUT = FDEV_SETUP_STREAM(uart0_putchar, NULL, _FDEV_SETUP_WRITE);

Clock_calculator clk_cal;

int main(void)
{
	init_i2c();
	init_timer0();
	init_uart0();
	init_keypad();
	init_button();
	init_lcd();
	init_my_clock(&(clk_cal.my_clock));
	init_my_calc(&(clk_cal.my_calc));

	stdout = &OUTPUT;
	sei();

	clk_cal.mode = CLOCK;	//시작 모드 CLOCK

	while (1)
	{
		button_processing(&clk_cal);
		if(clk_cal.mode == CLOCK)
			clock_processing(&(clk_cal.my_clock));
		else calc_processing(&(clk_cal.my_calc));
		lcd_processing(&clk_cal);
	}
}
