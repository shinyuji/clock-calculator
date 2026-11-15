/*
 * queue.h
 *
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include <stdint.h>

#define TRUE 1
#define FALSE 0
#define QUEUE_MAX 100

typedef struct _Queue{
	int front;   // read index
	int rear;    // insert index
	uint8_t data[QUEUE_MAX];
} Queue;

void init_queue(Queue *q);
int queue_full(Queue *q);
int queue_empty(Queue *q);
uint8_t read_queue(Queue *q);
void insert_queue(Queue *q, uint8_t value);

#endif /* QUEUE_H_ */
