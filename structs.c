void init_answer(Answer *answer) {
	pthread_mutex_init(&answer->mutex, NULL);
    pthread_cond_init(&answer->cond, NULL);
    answer->status = 0;
    answer->comment = NULL;
   	answer->value = 0;
}

void init_op(Operation *op) {
	op->put_answer_to = NULL;
	op->status = 0;
	pthread_mutex_init(op->mutex, NULL);
}

void fill_op(Operation *op, Answer *put_answer_to, OpCode code, int client_id) {
	op->put_answer_to = put_answer_to;
	op->code = code;
	op->client_id = client_id;
	op->status = 1;
}

Operation *clone_op(Operation *op) {
	Operation *result = (Operation*)malloc(sizeof(Operation));
	init_operation(result);
	
	result->put_answer_to = op->put_answer_to;
	result->code = op->code;
	result->client_id = op->client_id;
	result->status = op->status;

	return result;
}
