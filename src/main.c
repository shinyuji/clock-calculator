/*
 * LCD_clock.c
 */ 
#include "common.h"

extern void init_keypad(void);
extern void init_lcd(void);
extern void init_clock(Clock* clk);
extern void init_timer0(void);
extern void init_uart0(void);
extern void init_my_clock(Clock* clk);
extern void init_my_calc(Calculator *calc);

extern void lcd_processing(Clock_calculator *clk_cal);
extern void clock_processing(Clock* clk);
extern void calc_processing(Calculator *calc);
extern void button_processing(Clock_calculator *clk_cal);

extern void UART0_transmit(uint8_t data);

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

Clock_calculator clk_cal;

int main(void)
{	
	init_i2c();
	init_timer0();
	init_uart0();
	init_keypad();
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




