/*
 * I2C_MASTER.c
 *
 * ATmega128A TWI(I2C) 관련 register
 * TWCR: TWI Control Register
 *   TWINT: 작업 완료 flag (1을 쓰면 clear 후 다음 동작 시작)
 *   TWEA : ACK enable (1: 수신 후 ACK, 0: NACK)
 *   TWSTA: START 조건 생성 (Master)
 *   TWSTO: STOP 조건 생성 (Master)
 *   TWEN : TWI 활성화
 * TWSR: TWI Status Register (상위 5bit: 상태 코드, 하위 2bit: 분주비)
 * TWBR: TWI Bit Rate Register
 * TWDR: TWI Data Register
 */

#include "I2C_MASTER.h"

void init_i2c(void)
{
	TWSR = 0x00;
	TWBR = TWBR_VALUE;	//SCL 주파수 결정 register, 100KHz
}

//START 신호
//I2C 통신의 시작 신호를 만드는 코드.
uint8_t i2c_start(void)
{
	uint8_t st;
	//하드웨어에게 어떻게 시작할지를 지시
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;
	//TWINT: TWSTA: START 요청 TWEN: I2C HW 활성화
	//TWINT에 1을 주면, flag가 초기화 되는것(0으로 set)
	//TWSTA 비트를 1로 만들면 mcu의 twi하드웨어가 sda 라인과 scl라인을 강제로 조작해서 start신호를 보냄

	//I2C HW가 동작이 완료 될 때 까지 기다린다. 동작이 완료 되면 TWINT flag가 1로 set된다.
	while(!(TWCR &(1 << TWINT)));
	st = TWSR & 0xf8;	//7~3번 bit까지의 값을 취함 => 이게 twi의 중간 통신 상태들이 저장되는 비트.
	return (st == TWSR_START  || st == TWSR_REP_START) ? 0: st;	//성공이면 0, 아니면 에러 상태 코드 반환
}

//STOP 신호
void i2c_stop(void)
{
	TWCR = 1 << TWINT | 1 << TWSTO | 1 << TWEN;
	//TWINT: 동작 시작, TWSTO: STOP 요청, TWEN: I2C HW 활성화
	//STOP은 완료 후 TWINT가 set되지 않으므로 짧게 대기
	_delay_us(10);
}

//SLAVE_ADDR+R, SLAVE_ADDR+W
uint8_t i2c_slave_addr_send(uint8_t addr_rw)
{
	//어떤 슬레이브랑 대화할 것인지, 데이터를 줄 것인지, 받을 것인지 결정.
	uint8_t st;
	TWDR = addr_rw;	//TWI 데이터 레지스터.
	//슬레이브 주소값(7비트)과 읽기쓰기모드 비트(1비트)를 합친 값을 넣는다.*인자로  (SLAVE_ADDR << 1) | 1 을 전달함..
	TWCR = 1 << TWINT| 1 << TWEN;
	//TWINT: 동작 시작, TWEN: I2C HW 활성화
	//I2C HW가 동작이 완료 될 때 까지 기다린다. 동작이 완료 되면 TWINT flag가 1로 set된다.
	while(!(TWCR &(1 << TWINT)));
	st = TWSR & 0xf8;	//7~3번 bit까지의 값을 취함 (통신이 정상적으로 완료되었는지 상태 체크)
	return (st);	//상태를 return
}

uint8_t i2c_data_write(uint8_t data)
{
	TWDR = data;	//TWI DATA Register . 전송하고 싶은 값을 이 레지스터에 넣는다.

	TWCR = 1 << TWINT| 1 << TWEN;
	//TWINT를 1로 써서 전송 시작. HW가 SDA로 1bit씩 내보낸다.

	//I2C HW가 동작이 완료 될 때 까지 기다린다. 동작이 완료 되면 TWINT flag가 1로 set된다.
	while(!(TWCR &(1 << TWINT)));

	return (TWSR & 0xf8);	//상태 코드 반환 (성공: TWSR_MT_DATA_ACK)
}

uint8_t i2c_data_read_acksend(void)
{
	//read에서 필요한 것: 인터럽트, Enable, ACK 자동 전송
	TWCR = 1 << TWINT | 1 << TWEN | 1 << TWEA;
	// 1 << TWEA 이거를 twcr 레지스터에 넣으면 데이터를 받았으니 다음 데이터를 계속 보내달라는 요청이 됨.

	//TWINT: 동작 시작, TWEN: I2C HW 활성화
	//I2C HW가 동작이 완료 될 때 까지 기다린다. 동작이 완료 되면 TWINT flag가 1로 set된다.
	while(!(TWCR &(1 << TWINT)));
	return (TWDR);
}

uint8_t i2c_data_read_nacksend(void)
{
	//마지막 read에서 필요한 것: 인터럽트, Enable, NACK 자동 전송
	TWCR = 1 << TWINT | 1 << TWEN;
	//I2C규약상, 읽기 작업이 끝나면 마스터가 NACK을 보내야 함.
	//TWINT: 동작 시작, TWEN: I2C HW 활성화
	//I2C HW가 동작이 완료 될 때 까지 기다린다. 동작이 완료 되면 TWINT flag가 1로 set된다.
	while(!(TWCR &(1 << TWINT)));
	return (TWDR);
}
