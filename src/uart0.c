/*
 * uart0.c
 *
 * 디버그 메시지(printf) 출력용 UART0 송신
 */
#include "uart0.h"

/*
1. 전송속도: 9600bps
2. 송신만 사용 (printf 출력)
*/
void init_uart0(void)
{
	UBRR0H = 0x00;
	UBRR0L = 207;	//9600bps (U2X0 = 1)
	UCSR0A |= 1 << U2X0;	//2배속 설정 (sampling 8)
	UCSR0B |= 1 << TXEN0;	//송신 enable
}

//UART0로 1byte를 전송 하는 함수
void UART0_transmit(uint8_t data)
{
	while(!(UCSR0A & 1 << UDRE0))	//data가 송신중이면 송신이 끝날때 까지 기다림
		;	//no operation
	UDR0 = data;	// HW 전송 register에 data를 송신 한다.
}

//printf 출력을 UART0로 연결하는 함수
int uart0_putchar(char c, FILE *stream)
{
	(void)stream;
	UART0_transmit((uint8_t)c);
	return 0;
}
