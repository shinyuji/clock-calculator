/*
 * common.h
 *
 * Created: 2026-07-01 오후 4:28:30
 *  Author: kccistc
 */ 


#ifndef COMMON_H_
#define COMMON_H_

#include "queue.h"

#define F_CPU 16000000UL  // 16MHz
#include <avr/io.h>  // PORTA PORTB PORTD... IO관련 reg가 들어 있다.
#include <util/delay.h>  // _delay_ms _delay_us 등
#include <avr/interrupt.h>
#include <string.h>

typedef enum{
	CLK_SET_YEAR,
	CLK_SET_MONTH,
	CLK_SET_DATE,
	CLK_SET_HOUR,
	CLK_SET_MIN,
	CLK_SET_SEC,
}  CLK_SET_OPTION;

typedef enum{
	CLOCK_RUNNING,
	CLOCK_SETTING
}  CLOCK_MODE;

typedef struct _Clock{
	CLOCK_MODE clock_mode;
	CLK_SET_OPTION clk_set_option;
	int sec;
	int min;
	int hour;
	int day;
	int date;
	int month;
	int year;
} Clock;

typedef struct _Calculator{
	char inbuff[100];
	int index;
	Queue calc_data;
	
	int result;
	int error_flag;
} Calculator;

typedef enum{
	CLOCK,
	CALCULATOR
} MODE;

typedef struct _Clock_calculator
{
	MODE mode;
	Calculator my_calc;
	Clock my_clock;
	
} Clock_calculator;

#endif /* COMMON_H_ */