#include "protocol/historyStack.h"

#include <stdio.h>
#include <string.h>

#include "util/memory.h"

void
protocol_HistoryStack_init(struct protocol_HistoryStack *history)
{
	util_Stack_init(&history->stack);
}

void
protocol_HistoryStack_deinit(struct protocol_HistoryStack *history)
{
	util_Stack_deinit(&history->stack);
}

void
protocol_HistoryStack_push(struct protocol_HistoryStack *history,
		const char *url)
{
	char *newUrl = util_memory_calloc(sizeof(char), strlen(url) + 1);
	strcpy(newUrl, url);
	util_Stack_push(&history->stack, newUrl);
}

char *
protocol_HistoryStack_pop(struct protocol_HistoryStack *history)
{
	return (char *) util_Stack_pop(&history->stack);
}

void *
protocol_HistoryStack_forward(struct protocol_HistoryStack *history)
{
	return (char *) util_Stack_forward(&history->stack);
}

bool
protocol_HistoryStack_enableForward(struct protocol_HistoryStack *history)
{
	return util_Stack_hasForward(&history->stack);
}

bool
protocol_HistoryStack_enableBackward(struct protocol_HistoryStack *history)
{
	return (!util_Stack_isEmpty(&history->stack) && (history->stack.head != 1));
}

void
protocol_HistoryStack_print(struct protocol_HistoryStack *history)
{
	for (uint32_t i = 0; i < history->stack.head; ++i)
	{
		printf("%s\n", ((char *) history->stack.stack[i]));
	}
}

