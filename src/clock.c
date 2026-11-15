/*
 * clock.c
 *
 * Created: 2026-07-01 오후 6:21:47
 */
#include "clock.h"
#include "I2C_MASTER.h"
#include "lcd.h"

volatile uint8_t clock_read_flag = 0;

void init_my_clock(Clock *clk){
	//구조체 클락 초기화
	lcd_update_flag = 1;
	init_clock(clk);
	write_clock_data(clk);
}

void init_clock(Clock *clk)
{
	clk->sec = 30;
	clk->min = 20;
	clk->hour = 13;
	clk->day = 6;
	clk->date = 01;
	clk->month = 07;
	clk->year = 26;
}

uint8_t dec2bcd(uint8_t data)
{
	uint8_t high, low;

	high = (data/10) << 4;	// high nibble에 위치 e.g. 0010 0000
	low = data % 10;		// low nibble에 위치 e.g. 0000 0101
	return (high+low);
}

uint8_t bcd2dec(uint8_t data)
{
	uint8_t high, low;

	low = data & 0x0f;
	high = (data >> 4) * 10;
	return (high+low);
}

void write_clock_data(Clock *clk)
{
	uint8_t status;

	//=================읽기/쓰기 공통 시나리오====================

	// 1. START signal
	status = i2c_start();
	if(status) { printf("ERR: START(write) FAILED !!!\n"); return; }

	// 2. <전송> SLAVE_ADDR+W (W=0)
	status = i2c_slave_addr_send(SLAVE_ADDR << 1);
	if(status != TWSR_MT_SLA_ACK) { printf("ERR: SLAVE_ADDR(write) + NACK !!!\n"); i2c_stop(); return; }

	// 3. <쓰기> 접근할 레지스터 주소 (DS1307의 'seconds' register address: 00000000)
	i2c_data_write(0x00);

	//======================쓰기 시나리오======================

	// 4. <쓰기> clock data
	i2c_data_write(dec2bcd(clk->sec));
	i2c_data_write(dec2bcd(clk->min));
	i2c_data_write(dec2bcd(clk->hour));
	i2c_data_write(dec2bcd(clk->day));
	i2c_data_write(dec2bcd(clk->date));
	i2c_data_write(dec2bcd(clk->month));
	i2c_data_write(dec2bcd(clk->year));

	// 5. STOP signal
	i2c_stop();
}

void read_clock_data(Clock *clk)
{
	uint8_t rx_buff[8]= {0};
	uint8_t status;

	//==================읽기/쓰기 공통 시나리오===================

	// 1. START signal
	status = i2c_start();
	if(status) { printf("ERR: START(write) FAILED !!!\n"); return; }

	// 2. <전송> SLAVE_ADDR+W (W=0)
	status = i2c_slave_addr_send(SLAVE_ADDR << 1);
	if(status != TWSR_MT_SLA_ACK) { printf("ERR: SLAVE_ADDR(write) + NACK !!!\n"); i2c_stop(); return; }

	// 3. <쓰기> 접근할 레지스터 주소 (DS1307의 'seconds' register address: 00000000)
	status = i2c_data_write(0x00);

	//====================읽기 시나리오========================

	// 4. START signal
	status = i2c_start();
	if(status) { printf("ERR: START(read) FAILED !!!\n"); return; }

	// 5. <전송> SLAVE_ADDR+R (R=1)
	status = i2c_slave_addr_send((SLAVE_ADDR << 1) | 1);
	if(status != TWSR_MR_SLA_ACK) { printf("ERR: SLAVE_ADDR(read) + NACK !!!\n"); i2c_stop(); return; }

	// 6. <읽기> 시계 데이터 7바이트 수신 (sec, min, hour, day, date, month, year = 7bytes)
	for(int i = 0; i < 6; i++)
	{
		rx_buff[i] = i2c_data_read_acksend();		// 레지스터에 들어있는 데이터 수신 + ACK  전송(계속 보내라)
	}
	rx_buff[6] = i2c_data_read_nacksend();		// 레지스터에 들어있는 데이터 수신 + NACK 전송(그만 보내라)

	// rx_buff에 받아온 데이터를 구조체에 저장
	clk->sec = bcd2dec(rx_buff[0]);
	clk->min = bcd2dec(rx_buff[1]);
	clk->hour = bcd2dec(rx_buff[2] & 0b00111111);	//7,6번 bit(12/24시간 설정) 제외
	clk->day = bcd2dec(rx_buff[3]);
	clk->date = bcd2dec(rx_buff[4]);
	clk->month = bcd2dec(rx_buff[5]);
	clk->year = bcd2dec(rx_buff[6]);

	// 7. STOP signal
	i2c_stop();
}

void modify_clock_value(Clock *clk, int adj) {
	switch(clk->clk_set_option) {
		case CLK_SET_YEAR:
		clk->year += adj;
		if(clk->year > 99) clk->year = 0;
		if(clk->year < 0)  clk->year = 99;
		break;
		case CLK_SET_MONTH:
		clk->month += adj;
		if(clk->month > 12) clk->month = 1;
		if(clk->month < 1)  clk->month = 12;
		break;
		case CLK_SET_DATE:
		clk->date += adj;
		if(clk->date > 31) clk->date = 1;
		if(clk->date < 1)  clk->date = 31;
		break;
		case CLK_SET_HOUR:
		clk->hour += adj;
		if(clk->hour > 23) clk->hour = 0;
		if(clk->hour < 0)  clk->hour = 23;
		break;
		case CLK_SET_MIN:
		clk->min += adj;
		if(clk->min > 59) clk->min = 0;
		if(clk->min < 0)  clk->min = 59;
		break;
		case CLK_SET_SEC:
		clk->sec += adj;
		if(clk->sec > 59) clk->sec = 0;
		if(clk->sec < 0)  clk->sec = 59;
		break;
	}
}

void clock_processing(Clock *clk)
{
	//클락 플래그가 켜졌고 시계 모드의 시계가 동작중일 때,
	if(clock_read_flag && clk->clock_mode == CLOCK_RUNNING){
		clock_read_flag = 0;
		read_clock_data(clk);
		lcd_update_flag = 1;
	}
}
