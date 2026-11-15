/*
 * queue.c
 *
 */
#include "queue.h"

void init_queue(Queue *q)  // queue가 비어 있으면 front와 rear가 같은 위치를 가리킨다.
{
	q->front=-1;   // read index
	q->rear=-1;    // insert index
}

int queue_full(Queue *q)
{
	// queue에서 rear+1 % QUEUE_MAX의 값이 front와 같으면 queue full
	int tmp=(q->rear+1) % QUEUE_MAX;
	if (tmp == q->front)  // front와 같으면 queue full
		return TRUE;
	else return FALSE;
}

int queue_empty(Queue *q)
{
	if (q->rear == q->front)  // front와 같으면 queue empty
		return TRUE;
	else return FALSE;
}

uint8_t read_queue(Queue *q)
{
	if (queue_empty(q))
		return 0;	// 읽을 데이터 없음

	q->front = (q->front+1) % QUEUE_MAX;
	return (q->data[q->front]);
}

void insert_queue(Queue *q, uint8_t value)
{
	if (queue_full(q))
		return;	// 가득 차면 입력 무시 (인터럽트에서 호출되므로 출력하지 않음)

	q->rear = (q->rear+1) % QUEUE_MAX;
	q->data[q->rear]=value;
}
