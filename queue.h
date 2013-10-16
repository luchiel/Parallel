#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include "structs.h"

typedef struct Node {
	Operation *op;
	struct Node *right;
} OpNode;

typedef struct {
	OpNode *head;
	OpNode *tail;
	pthread_mutex_t mutex;
} OpQueue;

void init_opqueue(OpQueue *q);
void push_op(OpQueue *q, Operation *op);
Operation *pop_op(OpQueue *q);

#endif
