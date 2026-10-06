/*
 * urart0.h
 */ 

#ifndef URART0_H_
#define URART0_H_

#define F_CPU 16000000UL  // 16MHz
#include <avr/io.h>  // PORTA PORTB PORTD... IO관련 reg가 들어 있다.
#include <util/delay.h>  // _delay_ms _delay_us 등
#include <avr/interrupt.h>
#include <string.h>

#define QUEUE_SIZE 10
#define QUEUE_LENGTH 80
volatile int u_rear=0;  // interrupt에서 queue에 data를 저장하는 위치값
volatile int u_front=0;  // pc_command_processing에서 가져가는 위치값
volatile unsigned char rx_buff[QUEUE_SIZE][QUEUE_LENGTH];

#endif /* URART0_H_ */