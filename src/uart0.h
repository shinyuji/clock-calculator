/*
 * uart0.h
 */

#ifndef UART0_H_
#define UART0_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>

void init_uart0(void);
void UART0_transmit(uint8_t data);
int uart0_putchar(char c, FILE *stream);

#endif /* UART0_H_ */
