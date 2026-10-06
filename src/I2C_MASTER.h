/*
 * I2C_MASTE=R.h
 *
 * Created: 2026-07-01 오후 1:46:11
 *  Author: kccistc
 */ 

#ifndef I2C_MASTER_H_
#define I2C_MASTER_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>	//sei 함수 등
#include <stdio.h>	//printf
#include <string.h>

// I2C 설정

#define  I2C_FREQUENCY 100000UL	//100kHz의 speed
// TWBR: I2C 통신 bit rate 설정
// TWBR: (F_CPU / I2C_FREQUENCY - 16) / 2 (분주비 = 1)
#define  TWBR_VALUE (F_CPU / I2C_FREQUENCY - 16) / 2	//TWBR(TWI Bit Rate Register)

//------------ TWI 상태 코드(I2C 통신의 상태를 알려주는 코드) --------------
// TWS (TWI Status Register): 7번~3번 bits(5bit를 참조) -> I2C 통신의 전송 상태나 오류를 나타내는 register
#define TWSR_START 0x08	//START condition set
#define TWSR_REP_START 0x10
#define TWSR_MT_SLA_ACK 0x18	//Master가 slave에게 addr+w 전송 후, ACK 수신
#define TWSR_MT_DATA_ACK 0x28	//Master가 Slave에게 data 송신 후, ACK 수신
#define TWSR_MR_SLA_ACK 0x40	//Master가 slave에게 addr+r 전송 후, ACK 수신
#define TWSR_MR_DATA_ACK 0x50	//Master가 Slave로부터 data 수신 후, ACK 전송
#define TWSR_MR_DATA_NACK 0x58	//Master가 Slave로부터 data 수신 후, NACK 전송 (마지막)

#endif /* I2C_MASTER_H_ */