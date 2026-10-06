/*
 * timer.c
 *
 * Created: 2026-07-01 오후 8:53:33
 *  Author: kccistc
 */ 
#include "common.h"
#include "queue.h"

extern Clock_calculator clk_cal;
extern uint8_t keypad_scan(void);
extern void insert_queue(Queue *q, uint8_t value);

volatile uint32_t keypad_counter = 0;	//60ms를 재기 위해
volatile int ms_count= 0;
extern uint8_t clock_read_flag;
ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6;	// TCNT0 6~256: 250개 펄스 count하기 위해
	
	volatile uint8_t keydata = 0;
	
	if(++keypad_counter >= 60)	//60ms reached
	{
		clock_read_flag = 1;	//시계 데이터 읽기 신호
		keypad_counter = 0;
		if((keydata = keypad_scan())) //keypad를 check해서 눌려진 것이 있으면
		{							//circular queue에 저장 한다.
			insert_queue(&(clk_cal.my_calc.calc_data), keydata);
		}
	}
	ms_count++;
}

void init_timer0(void)
{
	TCNT0 = 6;	// 6~256: 250개의 펄스를 count하기 위해 6으로 세팅

	TCCR0 &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));	//0분주 - reset
	TCCR0 |= 1 << CS02;		//64분주
	
	TIMSK |= 1 << TOIE0;	// TIMER0 Overflow INT(256이 되는 순간 인터럽트)
}
