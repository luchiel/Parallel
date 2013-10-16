#include <pthread.h>
#include <stdlib.h>
#include "queue.h"

void init_opqueue(OpQueue *q) {
	q->head = NULL;
	q->tail = NULL;
	pthread_mutex_init(&q->mutex, NULL);
}

void push_op(OpQueue *q, Operation *op) {
	pthread_mutex_lock(&q->mutex);

	OpNode *node = (OpNode*)malloc(sizeof(OpNode));
	node->op = op;
	node->right = NULL;

	if(q->head == NULL) {
		q->head = node;
		q->tail = node;
	}
	else {
		q->tail->right = node;
		q->tail = node;
	}

	pthread_mutex_unlock(&q->mutex);
}

Operation *pop_op(OpQueue *q) {
	if(q->head == NULL)
		return NULL;

	pthread_mutex_lock(&q->mutex);

	OpNode *head = q->head;
	q->head = head->right;
	if(q->head == NULL)
		q->tail = NULL;
	
	Operation *result = head->op;
	free(head);

	pthread_mutex_unlock(&q->mutex);
	
	return result;
}
