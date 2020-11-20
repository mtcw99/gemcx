#ifndef UTIL_STACK_H
#define UTIL_STACK_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

struct util_Stack
{
	void **stack;
	int32_t head;
	int32_t maxHead;
	uint32_t alloc;
};

void util_Stack_init(struct util_Stack *const restrict stack);
void util_Stack_deinit(struct util_Stack *const restrict stack);

void util_Stack_push(struct util_Stack *const restrict stack,
		void *data);
void *util_Stack_pop(struct util_Stack *const restrict stack);
void *util_Stack_forward(struct util_Stack *const restrict stack);
bool util_Stack_hasForward(struct util_Stack *const restrict stack);
bool util_Stack_isEmpty(struct util_Stack *const restrict stack);

#endif // UTIL_STACK_H

