/*
 * lcd.h
 *
 * Created: 2026-07-01 오후 3:01:05
 *  Author: kccistc
 */ 

#ifndef LCD_H_
#define LCD_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// 데이터 버스를 PORTA로 변경 (ISP 통신 간섭 해결)
#define PORT_DATA		PORTB
#define PORT_CONTROL	PORTC
#define DDR_DATA		DDRB	
#define DDR_CONTROL		DDRC
#define RS_PIN			0
#define RW_PIN			1
#define E_PIN			2


//텍스트 LCD 모듈의 명령어
#define COMMAND_CLEAR_DISPLAY	0x01	//공백 문자로 화면을 지우고 커서를 홈 위치(주소 0번)로 이동시킨다.
#define COMMAND_8_BIT_MODE		0x38	//데이터 비트 크기, 디스플레이 행 수, 폰트 크기 설정. 0011 1000
#define COMMAND_4_BIT_MODE		0x28	//데이터 비트 크기, 디스플레이 행 수, 폰트 크기 설정. 0010 1000

/*
				|  RS | R/W | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 |
function set	|  0  |  0  |  0  |  0  |  1  |  DL |  N  |  F  |  -  |  -  |

[데이터 비트 크기, 디스플레이 행 수, 폰트 크기 설정]
DL: 8비트(1,high), 4비트(0,low)
N:	디스플레이 행 수
F:	폰트 크기

| N  |	F  | 행 수  | 폰트 크기
------------------------------
  0      0	   1       5*8
  0      1	   1       5*10
  1      -	   2       5*8		//2행 출력에서는 5*10크기 폰트 사용 불가

0011 1000 => 8비트, 2행 5*8폰트를 사용하겠다.
0010 1000 => 4비트, 2행 5*8폰트를 사용하겠다.
*/

#define COMMAND_DISPLAY_ON_OFF_BIT	2
#define COMMAND_CURSOR_ON_OFF_BIT	1
#define COMMAND_BLINK_ON_OFF_BIT	0

/*
				|  RS | R/W | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 |
display on/off	|  0  |  0  |  0  |  0  |  0  |  0  |  1  |  D  |  C  |  B  |

[디스플레이, 커서, 커서 깜빡임, on/off 설정]
D: 디스플레이
C: 커서
B: 커서 깜빡임

0000 1100 => 디스플레이 on
0000 1110 => 디스플레이 on, 커서 on

*/

/*
				|  RS | R/W | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 |
Entry mode set	|  0  |  0  |  0  |  0  |  0  |  0  |  0  |  1  | I/D |  S  |

[데이터 R or W 후, 메모리의 증가/감소 방향을 지정(출력 후 커서 이동, 화면 이동)]
I/D: 커서를 오른쪽으로(1,high), 커서를 왼쪽으로(0,low)
S:	I/D 값에 따라 디스플레이를 왼쪽 또는 오른쪽으로 옮김. (커서는 고정된 위치에 나타남)

0000 0110  => 커서를 오른쪽으로 옮기고(DDRAM의 주소 증가), 디스플레이 이동은 없음

*/



#endif /* LCD_H_ */