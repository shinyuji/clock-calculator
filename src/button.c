/*
 * button.c
 *
 * Created: 2026-06-10 오후 1:25:20
 */
#include "button.h"
#include "clock.h"
#include "calc.h"
#include "lcd.h"

static int get_button(int button_num, int button_pin);

// 버튼 초기화
void init_button(void)
{
	BUTTON_DDR &= ~(1 << BUTTON0PIN | 1 << BUTTON1PIN | 1 << BUTTON2PIN | 1 << BUTTON3PIN | 1 << BUTTON4PIN );
}

static int get_button(int button_num, int button_pin)
{
	// static: 함수를 빠져나와도 버튼별 이전 상태 유지
	static unsigned char button_status[BUTTON_NUMBER] = {BUTTON_RELEASE};
	int current_state;

	// 1. button을 읽는다.
	current_state = BUTTON_PIN & (1 << button_pin);
	// 2. 버튼 상태 check
	if (current_state  && button_status[button_num] == BUTTON_RELEASE)  // 버튼이 처음 눌려진 상태
	{
		_delay_ms(15);  // noise가 지나가기를 기다린다.
		button_status[button_num] =BUTTON_PRESS;
		return 0;     // 아직은 완전히 눌렀다 뗀 상태가 아니다.
	}
	else if (button_status[button_num] == BUTTON_PRESS && current_state == BUTTON_RELEASE)
	{  // 버튼이 이전에 눌려진 상태였으며 지금은 떼어진 상태
		button_status[button_num] = BUTTON_RELEASE;   // 다음 버튼을 체크 하기 위하여 초기화
		_delay_ms(15);  // noise가 지나가기를 기다린다.
		return 1;   // 완전히 1번 눌렀다 뗀 상태로 인정 한다.
	}

	return 0;   // 버튼이 open 상태 이다.
}

//클락일 때 버튼 제어
static void clock_btn_processing(int btn_num, Clock_calculator *clk_cal){

	Clock *clk = &(clk_cal->my_clock);
	Calculator *calc = &(clk_cal->my_calc);

	switch(btn_num)
	{
		case 0:	//감소
			if(clk->clock_mode == CLOCK_SETTING){
				modify_clock_value(clk, -1);
			}
			break;
		case 1:	//증가
			if(clk->clock_mode == CLOCK_SETTING){
				modify_clock_value(clk, 1);
			}
			break;
		case 2:
			//날짜,시각 선택
			if(clk->clock_mode == CLOCK_SETTING){
				clk->clk_set_option = (clk->clk_set_option + 1)%6;
			}
			break;
		case 3:
			//날짜,시각 설정 <-> 시계 모드
			clk->clock_mode = !clk->clock_mode;
			clk->clk_set_option = CLK_SET_YEAR;	//초기값
			write_clock_data(clk);	//설정한 시각을 RTC에 저장
			break;
		case 4:
			//계산기 모드로 변경
			clk_cal->mode = CALCULATOR;
			init_my_calc(calc); //초기화
			break;
	}
	lcd_update_flag = 1;
}

//계산기일 때 버튼 제어
static void calc_btn_processing(int btn_num, Clock_calculator *clk_cal){

	Calculator *calc = &(clk_cal->my_calc);
	switch(btn_num)
	{
		case 0:	//계산기 reset
			init_my_calc(calc); //초기화
			break;
		case 1:	//큐에 ' '삽입(backspace)
			insert_queue(&(calc->calc_data),' ');
			break;
		case 2:	//큐에 '(' 삽입
			insert_queue(&(calc->calc_data),'(');
			break;
		case 3:	//큐에 ')' 삽입
			insert_queue(&(calc->calc_data),')');
			break;
		case 4:
			//시계 모드로 변경
			clk_cal->mode = CLOCK;
			break;
	}
	lcd_update_flag = 1;
}

//버튼 이벤트 발생 확인
void button_processing(Clock_calculator *clk_cal){
	//버튼은 2개의 모드에서 사용
	int btn_num = -1;

	if (get_button(BUTTON0,BUTTON0PIN)) {btn_num = 0;}
	if (get_button(BUTTON1,BUTTON1PIN)) {btn_num = 1;}
	if (get_button(BUTTON2,BUTTON2PIN)) {btn_num = 2;}
	if (get_button(BUTTON3,BUTTON3PIN)) {btn_num = 3;}
	if (get_button(BUTTON4,BUTTON4PIN)) {btn_num = 4;}

	if(btn_num == -1) return;

	//함수 포인터 배열 방식으로 처리
	void (*fp[])(int, Clock_calculator*) = {clock_btn_processing, calc_btn_processing};

	//각 모드별 버튼 처리 함수로 넘기기
	fp[clk_cal->mode](btn_num, clk_cal);
}
