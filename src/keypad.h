/*
 * keypad.h
 *
 * Created: 2026-06-29 오후 2:11:08
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#define F_CPU 16000000UL  // 16MHz
#include <avr/io.h>  // PORTA PORTB PORTD... IO관련 reg가 들어 있다.
#include <util/delay.h>  // _delay_ms _delay_us 등
#include <avr/interrupt.h>

#define KEYPAD_DDR DDRA
#define KEYPAD_PIN PINA
#define KEYPAD_PORT PORTA

void init_keypad(void);
uint8_t keypad_scan(void);

#endif /* KEYPAD_H_ */
