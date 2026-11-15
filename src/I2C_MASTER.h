/*
 * I2C_MASTER.h
 */

#ifndef I2C_MASTER_H_
#define I2C_MASTER_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// I2C 설정
#define I2C_FREQUENCY 100000UL	// 100kHz
// TWBR = (F_CPU / I2C_FREQUENCY - 16) / 2 (분주비 1)
#define TWBR_VALUE ((F_CPU / I2C_FREQUENCY - 16) / 2)

// TWSR 상태 코드 (상위 5bit)
#define TWSR_START			0x08	// START 전송 완료
#define TWSR_REP_START		0x10	// 반복 START 전송 완료
#define TWSR_MT_SLA_ACK		0x18	// SLA+W 전송 후 ACK 수신
#define TWSR_MT_DATA_ACK	0x28	// 데이터 전송 후 ACK 수신
#define TWSR_MR_SLA_ACK		0x40	// SLA+R 전송 후 ACK 수신
#define TWSR_MR_DATA_ACK	0x50	// 데이터 수신 후 ACK 전송
#define TWSR_MR_DATA_NACK	0x58	// 데이터 수신 후 NACK 전송 (마지막 바이트)

void init_i2c(void);
uint8_t i2c_start(void);
void i2c_stop(void);
uint8_t i2c_slave_addr_send(uint8_t addr_rw);
uint8_t i2c_data_write(uint8_t data);
uint8_t i2c_data_read_acksend(void);
uint8_t i2c_data_read_nacksend(void);

#endif /* I2C_MASTER_H_ */
