#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
#include "queue.h"

#define CLIENT_COUNT 5
#define CALL_COUNT 10

Operation operation;
Answer answers[CLIENT_COUNT];
pthread_mutex_t print_mutex;

int can_perform_op(OpCode code, int client_id, int locked_by) {
	// 1 = ok, 0 = later, -1 = error
	switch(code) {
		case OP_LOOK:
			return 1;
		case OP_LOCK:
			return locked_by == client_id || locked_by == -1 ? 1 : 0;
		case OP_UNLOCK:
		case OP_INC:
		case OP_DEC:
			return locked_by == client_id ? 1 : -1;
	}
	return 0;
}

char *code_to_text(OpCode code) {
	switch(code) {
		case OP_LOCK: return "LOCK";
		case OP_UNLOCK: return "UNLOCK";
		case OP_LOOK: return "LOOK";
		case OP_INC: return "INC";
		case OP_DEC: return "DEC";
	}
	return "unknown op";
}

void *bank_server(void *args) {
	OpQueue queue;
	init_opqueue(&queue);

	int balance = 0;
	int locked_by = -1;
	while(1) {
		Operation *op = NULL;

		if(operation.status == 1) {
			pthread_mutex_lock(&operation.mutex);
			op = clone_op(&operation);
			operation.status = 0;
			pthread_mutex_unlock(&operation.mutex);

			fprintf(stderr, "[Thread %d], operation %s, new op", op->client_id, code_to_text(op->code));
		}
		else if(locked_by == -1) {
			op = pop_op(&queue);

			if(op != NULL)
			{
				fprintf(stderr, "[Thread %d], operation %s, popped op", op->client_id, code_to_text(op->code));
			}
		}
		else {
			continue;
		}

		if(op == NULL) {
			continue;
		}

		int is_performable = can_perform_op(op->code, op->client_id, locked_by);

		if(is_performable != 0) {
			pthread_mutex_lock(&op->put_answer_to->mutex);

			if(is_performable == 1) {
				switch(op->code) {
					case OP_LOOK:
						op->put_answer_to->comment = "";
						break;
					case OP_LOCK:
						op->put_answer_to->comment = "successfully locked";
						locked_by = op->client_id;
						break;
					case OP_UNLOCK:
						op->put_answer_to->comment = "successfully unlocked";
						locked_by = -1;
						break;
					case OP_INC:
						balance += op->value;
						op->put_answer_to->comment = "successfuly added";
						break;
					case OP_DEC:
						balance -= op->value;
						op->put_answer_to->comment = "successfully withdrawed";
						break;
				}
				fprintf(stderr, " - completed\n");
			}
			else {
				op->put_answer_to->comment = "balance not locked";

				fprintf(stderr, " - declined\n");
			}

			op->put_answer_to->status = is_performable;
			op->put_answer_to->value = balance;

			// free(op);

			pthread_cond_signal(&op->put_answer_to->cond);
			pthread_mutex_unlock(&op->put_answer_to->mutex);
		}
		else {
			push_op(&queue, op);

			fprintf(stderr, " - pushed to queue\n");
		}
	}
	return NULL;
}

void *bank_client(void *args) {
	int client_id = *(int*)args;
	Answer *answer = &answers[client_id];

	for(int i = 0; i < CALL_COUNT; ++i) {
		OpCode call_code = i + 1 != CALL_COUNT ?
			(OpCode)(rand() % 5) :
			OP_UNLOCK;
		int value = rand() % 100;

		while(operation.status != 0) {
		}
		pthread_mutex_lock(&operation.mutex);
		fill_op(&operation, answer, call_code, client_id, value);
		pthread_mutex_unlock(&operation.mutex);

		pthread_mutex_lock(&answer->mutex);
		while(answer->status == 0) {
			pthread_cond_wait(&answer->cond, &answer->mutex);
		}
		pthread_mutex_unlock(&answer->mutex);

		pthread_mutex_lock(&print_mutex);
		if(answer->status == 1) {
			switch(call_code) {
				case OP_LOOK:
					printf(
						"[Thread %d]%s current balance %d\n",
						client_id, answer->comment, answer->value);
					break;
				case OP_LOCK:
					printf("[Thread %d] %s\n", client_id, answer->comment);
					break;
				case OP_UNLOCK:
					printf("[Thread %d] %s\n", client_id, answer->comment);
					break;
				case OP_INC:
				case OP_DEC:
					printf(
						"[Thread %d] %s %d, current balance %d\n",
						client_id, answer->comment, value, answer->value);
					break;
			}
		}
		else {
			printf("[Thread %d] Error: %s (tried %s)\n", client_id, answer->comment, code_to_text(call_code));
		}

		fflush(stdout);
		pthread_mutex_unlock(&print_mutex);

		answer->status = 0;
	}
	return NULL;
}

int main() {
	pthread_t server_th;
	pthread_t client_ths[CLIENT_COUNT];

	init_op(&operation);
	pthread_mutex_init(&print_mutex, NULL);

	pthread_create(&server_th, NULL, bank_server, NULL);

	for(int i = 0; i < CLIENT_COUNT; ++i) {
		int *idx = (int*)malloc(sizeof(int));
		*idx = i;
		init_answer(&answers[i]);
		pthread_create(&client_ths[i], NULL, bank_client, idx);
	}

	pthread_join(server_th, NULL);

	return 0;
}
