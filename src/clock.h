/*
 * clock.h
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#define SLAVE_ADDR 0x68		//7bit slave(DS1307) 주소

#define ADDR_SEC		0x00	//datasheet 참고
#define ADDR_MIN		0x01
#define ADDR_HOUR		0x02
#define ADDR_DAY		0x03
#define ADDR_DATE		0x04
#define ADDR_MONTH		0x05
#define ADDR_YEAR		0x06
#define ADDR_CONTROL	0x07

#include "common.h"

extern volatile uint8_t clock_read_flag;	// Timer0 인터럽트에서 60ms마다 set

void init_my_clock(Clock *clk);
void init_clock(Clock *clk);
void write_clock_data(Clock *clk);
void read_clock_data(Clock *clk);
void modify_clock_value(Clock *clk, int adj);
void clock_processing(Clock *clk);

#endif /* CLOCK_H_ */
