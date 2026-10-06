/*
 * queue.h
 *
 */ 


#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define QUEUE_MAX 100

typedef struct _Queue{
	int front;   // read index
	int rear;    // insert index
	uint8_t data[QUEUE_MAX];
} Queue;

#endif /* QUEUE_H_ */