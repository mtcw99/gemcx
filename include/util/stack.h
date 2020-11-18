#ifndef UTIL_STACK_H
#define UTIL_STACK_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

struct util_Stack
{
	void **stack;
	uint32_t head;
	uint32_t alloc;
	uint32_t maxHead;
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

