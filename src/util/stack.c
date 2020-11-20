#include "util/stack.h"

#include "util/memory.h"

enum
{
	UTIL_STACK_ALLOC_SIZE = 8
};

void
util_Stack_init(struct util_Stack *const restrict stack)
{
	stack->stack = NULL;
	stack->alloc = 0;
	stack->head = -1;
	stack->maxHead = -1;
}

void
util_Stack_deinit(struct util_Stack *const restrict stack)
{
	if (stack->stack != NULL)
	{
		util_memory_free(stack->stack);
	}

	stack->stack = NULL;
	stack->alloc = 0;
	stack->head = -1;
	stack->maxHead = -1;
}

static void
util_Stack__expand(struct util_Stack *const restrict stack)
{
	if (stack->stack == NULL)
	{
		stack->alloc = UTIL_STACK_ALLOC_SIZE;
		stack->stack = util_memory_calloc(
				sizeof(void *), stack->alloc);
	}
	else if (stack->head == (stack->alloc - 1))
	{
		stack->alloc += UTIL_STACK_ALLOC_SIZE;
		stack->stack = util_memory_realloc(stack->stack,
				sizeof(void *) * stack->alloc);
	}
}

void
util_Stack_push(struct util_Stack *const restrict stack,
		void *data)
{
	util_Stack__expand(stack);
	stack->maxHead += (stack->head == stack->maxHead);
	stack->stack[++stack->head] = data;
}

void *
util_Stack_pop(struct util_Stack *const restrict stack)
{
	return (stack->head == -1) ? NULL : stack->stack[--stack->head];
}

void *
util_Stack_forward(struct util_Stack *const restrict stack)
{
	return (util_Stack_hasForward(stack)) ? stack->stack[++stack->head] : NULL;
}

bool
util_Stack_hasForward(struct util_Stack *const restrict stack)
{
	return (stack->head < stack->maxHead);
}

bool
util_Stack_isEmpty(struct util_Stack *const restrict stack)
{
	return (stack->head == -1);
}

