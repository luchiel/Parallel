#ifndef STRUCTS_H
#define STRUCTS_H

typedef enum { OP_LOCK, OP_UNLOCK, OP_LOOK, OP_INC, OP_DEC } OpCode;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int status; //waiting = 0, answered = 1, error = -1
    char *comment;
    int value;
} Answer;

typedef struct {
    OpCode code;
    Answer *put_answer_to;
    int client_id;
    int status; //empty = 0, filled = 1
    int value;
    pthread_mutex_t mutex;
} Operation;

void init_answer(Answer *answer);

void init_op(Operation *op);
void fill_op(Operation *op, Answer *put_answer_to, OpCode code, int client_id);
Operation *clone_op(Operation *op);

#endif
