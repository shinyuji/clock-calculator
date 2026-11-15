/*
 * calc.h
 *
 * Created: 2026-06-29 오후 4:15:27
 */

#ifndef CALC_H_
#define CALC_H_

#define STACK_SIZE 100
#define EMPTY -1

#include <stdio.h>
#include <string.h>
#include "common.h"

typedef struct {
	int top;
	int stack[100];
} number_stack;

typedef struct {
	int top;
	char stack[100];
} operator_stack;

enum error_code {
	SUCCESS,
	ERR_L_BRACKET,
	ERR_R_BRACKET,
	ERR_EMPTY_BRACKET,
	ERR_OPERATOR,
	ERR_EQUAL,
	ERR_INVALID,
	ERR_EMPTY
};

void init_my_calc(Calculator *calc);
void calc_processing(Calculator *calc);

#endif /* CALC_H_ */
