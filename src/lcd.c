/*
 * lcd.c
 *
 * Created: 2026-07-01 오후 3:00:51
 */

#include "lcd.h"

volatile uint8_t lcd_update_flag;

/************************************************************************/
/* 텍스트 LCD를 제어하기 위한 함수들                                     */
/************************************************************************/

//E핀에 한번의 펄스를 생성하는 함수 (하강 엣지에서 동작)
void LCD_pulse_enable(void)
{
	//E에 HIGH(1) 신호 주기
	PORT_CONTROL |= (1 << E_PIN);
	_delay_us(50);
	//E에 LOW(0) 신호 주기
	PORT_CONTROL &= ~(1 << E_PIN);
	_delay_us(50);
}

//LCD에 데이터 쓰는 함수
void LCD_write_data(uint8_t data)
{
	//데이터 레지스터(RS=1)로 set
	PORT_CONTROL |= (1 << RS_PIN);

	//4비트 데이터 넣기 - 1
	PORT_DATA = data & 0xF0;	//상위 4비트
	//E 펄스 (하강 에지에서 래치)
	LCD_pulse_enable();

	//4비트 데이터 넣기 - 2
	PORT_DATA = (data << 4) & 0xF0;	//하위 4비트
	//E 펄스 (하강 에지에서 래치)
	LCD_pulse_enable();

	_delay_ms(2);
}

//LCD에 명령어 쓰는 함수
void LCD_write_command(uint8_t command)
{
	//제어-명령어 레지스터(RS=0)로 set
	PORT_CONTROL &= ~(1 << RS_PIN);

	//4비트 명령어 넣기 - 1
	PORT_DATA = command & 0xF0;	//상위 4비트
	//E 펄스 (하강 에지에서 래치)
	LCD_pulse_enable();

	//4비트 명령어 넣기 - 2
	PORT_DATA = (command << 4) & 0xF0;	//하위 4비트
	//E 펄스 (하강 에지에서 래치)
	LCD_pulse_enable();

	_delay_ms(2);
}

//화면 clear하는 함수
void LCD_clear(void)
{
	LCD_write_command(COMMAND_CLEAR_DISPLAY);	//0000 0001
	_delay_ms(2);
}

/************************************************************************/
/* 4비트 모드로 텍스트 LCD를 초기화 하는 함수                             */
/************************************************************************/

void init_lcd(void)
{
	//초기 구동
	_delay_ms(50);

	DDR_DATA = 0xf0;	//데이터 핀 PB7~PB4를 출력으로 설정
	PORT_DATA = 0x00;	//데이터 포트의 출력핀들을 전부 0으로 set

	//port c가 제어핀 포트.. pc0, pc1, pc2를 출력핀으로 설정
	DDR_CONTROL |= (1 << RS_PIN) | (1 << RW_PIN) | (1 << E_PIN);
	//rw에 0; 쓰기 모드로(0)으로 set
	PORT_CONTROL &= ~(1 << RW_PIN);

	//rs: 제어 레지스터와 데이터 레지스터 중 선택: 명령 레지스터(0,low), 데이터 레지스터(1,high)
	//rw: 읽기쓰기모드: 읽기(1,high), 쓰기(0,low); 보통 거의 쓰기 모드..
	//e: 하강엣지에서 처리 시작.

	//4비트 모드 추가 명령
	/*
	0x02(0000 0010)를 전송하면 LCD_write_command가 nibble을 쪼개서 00000000과 00100000(4비트 모드로 세팅)라는 2개의 명령어로 나눔
	*/

	//command)function set:
	LCD_write_command(0x02); //0000 0010

	//command)function set: 0010 1000
	//4비트, 2행 5*8폰트로 설정
	LCD_write_command(COMMAND_4_BIT_MODE);

	//command)display on/off: 0000 1100 (뒤에 3자리만 가변)
	uint8_t command = 0x08 | (1 << COMMAND_DISPLAY_ON_OFF_BIT);
	LCD_write_command(command);

	//command)Clear display: 0000 0001
	LCD_clear();

	//command)Entry mode set: 커서를 오른쪽으로 옮기고(DDRAM의 주소 증가), 디스플레이 이동은 없음, 0000 0110
	//한번만 이니셜라이즈 해두면 글자 하나 입력 후, 오른쪽으로 커서 옮기는 것이 기본값으로 고정됨.
	LCD_write_command(0x06);
}

/************************************************************************/
/* 문자열 출력과 임의의 위치에 문자를 출력하기 위한 함수                   */
/************************************************************************/

//문자열 LCD로 출력하는 함수
void LCD_write_string(char *string)
{
	uint8_t i;

	//i에 '\0'이 오기 전까지 반복
	for(i = 0; string[i]; i++)
	{
		LCD_write_data(string[i]);
	}
}

//행과 열을 인자로 보내면 해당 위치로 커서를 옮기는 함수
void LCD_goto_XY(uint8_t row, uint8_t col)
{
	col %= 16;	//열의 범위 0~15
	row %= 2;	//행의 범위 0~1

	/*
	LCD | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 |
	| 0 |   |   |   |   |   |   |   |   |   |   |    |    |    |    |    |    |
	| 1 |   |   |   |   |   | * |   |   |   |   |    |    |    |    |    |    |
	*/

	//실제 칸의 메모리 주소(DDRAM 주소)를 계산하는 식
	uint8_t address = (0x40 * row) + col;

	/*
	<DATASHEET 참고>
	- LCD의 첫 번째 줄(0행)의 시작 주소는 0x00
	- LCD의 두 번째 줄(1행)의 시작 주소는 0x40

	ex) 2번째 줄, 6번째 칸(*)으로 가고 싶다면, (0x40*1)+5 = 0x45.
	*/

	//command)set DDRAM address: 001xxxxxxx (뒤에 7비트는 주소)
	uint8_t command = 0x80 + address;
	LCD_write_command(command);
}

//시계 화면 출력
void lcd_clock_print(Clock *clk)
{
	char date_lcd[17] = {0};
	char time_lcd[17] = {0};

	sprintf(date_lcd, "%02d-%02d-%02d        ", clk->year, clk->month, clk->date);
	sprintf(time_lcd, "%02d:%02d:%02d        ", clk->hour, clk->min, clk->sec);

	LCD_goto_XY(0, 0);
	_delay_ms(2);	//주소 설정 명령 처리 대기
	LCD_write_string(date_lcd);

	LCD_goto_XY(1, 0);
	_delay_ms(2);
	LCD_write_string(time_lcd);
}

void lcd_clock_modify_print(Clock *clk)
{
	lcd_clock_print(clk);

	switch(clk->clk_set_option) {
		case CLK_SET_YEAR:  LCD_goto_XY(0, 1); break;
		case CLK_SET_MONTH: LCD_goto_XY(0, 4); break;
		case CLK_SET_DATE:  LCD_goto_XY(0, 7); break;
		case CLK_SET_HOUR:  LCD_goto_XY(1, 1); break;
		case CLK_SET_MIN:   LCD_goto_XY(1, 4); break;
		case CLK_SET_SEC:   LCD_goto_XY(1, 7); break;
	}
}
//계산기 화면 출력
void lcd_calc_print(Calculator *calc)
{
	char display_buff[17] = {0};

	//실시간으로 입력 중인 수식 출력
	LCD_goto_XY(0, 0);
	_delay_ms(2);

	int adj;
	if (calc->index == 0) {
		LCD_write_string("Enter equation: "); //아무것도 입력 안 했을 때
		} else {
		//입력 중인 수식을 실시간으로 1행에 출력
			if(calc->index >16){
				//16자를 넘으면 마지막 16자만 표시
				adj = calc->index-16;
			} else adj = 0;
		sprintf(display_buff, "%-16.16s", (calc->inbuff)+adj);	//16개만 잘라서 가져오기

		LCD_write_string(display_buff);
	}

	//평소에는 비어있다가, =을 누르면 에러나 결과값 출력

	LCD_goto_XY(1, 0);
	_delay_ms(2);

	if (calc->state == CALC_ERROR) {
		//파싱 에러나 0으로 나누기 에러 시 2행에 출력
		LCD_write_string("= ERROR         ");
	}
	else if (calc->state == CALC_DONE) {
		//계산이 성공했을 때 2행에 결과값 출력
		sprintf(display_buff, "= %-14d", calc->result);
		LCD_write_string(display_buff);
	}
	else {
		//수식을 타이핑하는 중에는 2행을 비워둠
		LCD_write_string("                ");
	}
}

//lcd 화면 출력 (갱신 flag가 set된 경우에만)
void lcd_processing(Clock_calculator *clk_cal)
{
	Clock *clk = &(clk_cal->my_clock);

	if(!lcd_update_flag) return;
	lcd_update_flag = 0;

	if (clk_cal->mode == CALCULATOR)
	{
		lcd_calc_print(&(clk_cal->my_calc));
		return;
	}

	if (clk->clock_mode == CLOCK_SETTING)
	{
		lcd_clock_modify_print(clk);
		LCD_write_command(0x0F);   // 커서 ON + blink
	}
	else
	{
		lcd_clock_print(clk);
		LCD_write_command(0x0C);   // 커서 OFF
	}
}
