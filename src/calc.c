/*
 * calc.c
 *
 * Created: 2026-06-29 오후 4:15:16
 */

#include "calc.h"

static void string_calculator(Calculator *calc);
int parsing(char* inbuff, char* postfix);
int postfix_calculator(char* postfix, int* result);
void push_num(number_stack* p, int num);
int pop_num(number_stack* p);
void push_op(operator_stack* p, char op);
char pop_op(operator_stack* p);
int op_priority(char op);
int is_number(char ch);
int is_operator(char ch);

void init_my_calc(Calculator *calc){
	//계산기 구조체 초기화
	init_queue(&(calc->calc_data));
	memset(calc->inbuff, '\0', STACK_SIZE * sizeof(char));
	calc->index = 0;
	calc->state = CALC_INPUT;
}

void calc_processing(Calculator *calc){

	uint8_t key_value;

	//queue에 들어있는 char을 inbuff[]에 저장하다가 '='을 입력받으면 계산 실행
	if(queue_empty(&(calc->calc_data)) !=TRUE)
	{
		key_value = read_queue(&(calc->calc_data));	//queue에 있는 char을 하나 읽어서 받아오기

		// 결과가 나온 상태에서 새로운 키를 누르면 화면 전체 초기화
		if (calc->state != CALC_INPUT)
		{
			init_my_calc(calc);
		}

		if (key_value == ' ') {
			if (calc->index > 0) {
				calc->index--;						//delete
				calc->inbuff[calc->index] = '\0';
			}
		}
		//'='을 입력받으면 계산 실행
		else if(key_value == '='){
			calc->inbuff[calc->index] = '\0';
			string_calculator(calc);
		}
		else
		{
			if(calc->index < STACK_SIZE -1)
				calc->inbuff[calc->index++] = key_value;
		}
	}
}

//입력된 수식을 계산해 결과 또는 오류 상태 저장
static void string_calculator(Calculator *calc)
{
	char postfix[STACK_SIZE*3];
	int result;

	//중위표기식 문자열 -> 후위표기식 문자열
	if (parsing(calc->inbuff, postfix)) {
		calc->state = CALC_ERROR;
		return;
	}

	//후위표기식 계산기로 계산
	int cal_error = postfix_calculator(postfix, &result);

	if (cal_error == SUCCESS) {
		calc->result = result;
		calc->state = CALC_DONE;
	}
	else {
		calc->state = CALC_ERROR;
	}

}

//중위표기식 문자열 파싱 함수
int parsing(char* inbuff, char* postfix) {

	if (inbuff[0] == '\n' || inbuff[0] == '\0') return ERR_EMPTY;

	operator_stack op_stack;
	memset(&op_stack, 0, sizeof(operator_stack));
	operator_stack* p_stack = &op_stack;
	p_stack->top = EMPTY;

	char pre_char = '\0';
	int pf_i = 0;
	int bracket_count = 0;
	int equal = 0;

	int len = strlen(inbuff);

	for (int i = 0; i < len; i++) {
		char curr_char = inbuff[i];

		if (curr_char == ' ' || curr_char == '\n') continue;
		if (equal == 1) return ERR_OPERATOR;

		if (is_number(curr_char)) {

			if (pre_char == ')') return ERR_OPERATOR;

			while (i < len && is_number(inbuff[i])) {
				postfix[pf_i++] = inbuff[i];
				pre_char = inbuff[i];
				i++;
			}
			postfix[pf_i++] = ' ';
			i--;
		}

		else if (is_operator(curr_char)) {

			if (is_operator(pre_char) || pre_char == '=' || pre_char == '(' || pre_char == '\0') return ERR_OPERATOR;

			while (p_stack->top != EMPTY && op_priority(p_stack->stack[p_stack->top]) >= op_priority(curr_char)) {
				postfix[pf_i++] = pop_op(p_stack);
				postfix[pf_i++] = ' ';
			}
			push_op(p_stack, curr_char);
			pre_char = curr_char;
		}

		else if (curr_char == '(') {

			if (is_number(pre_char)) return ERR_OPERATOR;

			bracket_count++;
			push_op(p_stack, curr_char);
			pre_char = curr_char;
		}

		else if (curr_char == ')') {
			bracket_count--;

			if (bracket_count < 0) return ERR_R_BRACKET;
			if (pre_char == '(') return ERR_EMPTY_BRACKET;

			if (is_operator(pre_char)) return ERR_OPERATOR;

			while (p_stack->top != EMPTY && p_stack->stack[p_stack->top] != '(') {
				postfix[pf_i++] = pop_op(p_stack);
				postfix[pf_i++] = ' ';
			}
			pop_op(p_stack);
			pre_char = curr_char;
		}

		else if (curr_char == '=') {

			if (is_operator(pre_char)) return ERR_OPERATOR;

			equal = 1;
			pre_char = curr_char;
		}
		else return ERR_INVALID;

	}

	if (bracket_count > 0) return ERR_L_BRACKET;

	while (p_stack->top != EMPTY) {
		postfix[pf_i++] = pop_op(p_stack);
		postfix[pf_i++] = ' ';
	}
	postfix[pf_i] = 0;

	return SUCCESS;
}

//후위표기식 문자열 계산기
int postfix_calculator(char* postfix, int* result) {
	number_stack s_num1;
	memset(&s_num1, 0, sizeof(number_stack));
	number_stack* ps_num = &s_num1;

	int curr_num = 0;
	int is_num = 0;
	int len = strlen(postfix);

	for (int i = 0; i < len; i++) {
		char curr_char = postfix[i];

		if (is_number(curr_char)) {
			curr_num = (curr_num * 10) + (curr_char - '0');
			is_num = 1;
		}
		else {
			if (is_num == 1) {
				push_num(ps_num, curr_num);
				curr_num = 0;
				is_num = 0;
			}

			if (is_operator(curr_char)) {
				int right = pop_num(ps_num);
				int left = pop_num(ps_num);
				switch (curr_char) {
				case '+':
					left = left + right;
					break;
				case '-':
					left = left - right;
					break;
				case '*':
					left = left * right;
					break;
				case '/':
					if (right == 0) {
						return ERR_INVALID;
					} else left = left / right;
					break;
				}
				push_num(ps_num, left);
			}
		}
	}
	*result = pop_num(ps_num);
	return SUCCESS;
}

void push_num(number_stack* p, int num) {
	if (p->top != STACK_SIZE - 1) {
		p->top++;							//top + 1
		p->stack[p->top] = num;
	}
}
int pop_num(number_stack* p) {
	if (p->top != EMPTY) {
		int pop_num = p->stack[p->top];
		p->top--;							//top - 1
		return pop_num;
	}
	return 0;
}

void push_op(operator_stack* p, char op) {
	if (p->top != STACK_SIZE - 1) {
		p->top++;							//top + 1
		p->stack[p->top] = op;
	}
}

char pop_op(operator_stack* p) {
	if (p->top != EMPTY) {
		char pop_op = p->stack[p->top];
		p->top--;							//top - 1
		return pop_op;
	}
	return 0;
}

int op_priority(char op) {
	if (op == '*' || op == '/') return 3;
	if (op == '+' || op == '-') return 2;
	if (op == '(') return 1;
	return 0;
}

int is_number(char ch) {
	if (ch >= '0' && ch <= '9') {
		return 1;
	}
	else return 0;
}

int is_operator(char ch) {
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
		return 1;
	}
	else return 0;
}
