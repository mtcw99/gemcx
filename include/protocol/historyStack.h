#ifndef PROTOCOL_HISTORYSTACK_H
#define PROTOCOL_HISTORYSTACK_H

#include "util/stack.h"
#include "util/memory.h"

struct protocol_HistoryStack
{
	struct util_Stack stack;
};

void protocol_HistoryStack_init(struct protocol_HistoryStack *history);
void protocol_HistoryStack_deinit(struct protocol_HistoryStack *history);

void protocol_HistoryStack_push(struct protocol_HistoryStack *history,
		const char *url);
char *protocol_HistoryStack_pop(struct protocol_HistoryStack *history);
void *protocol_HistoryStack_forward(struct protocol_HistoryStack *history);
bool protocol_HistoryStack_enableForward(struct protocol_HistoryStack *history);
bool protocol_HistoryStack_enableBackward(struct protocol_HistoryStack *history);

void protocol_HistoryStack_print(struct protocol_HistoryStack *history);

#endif // PROTOCOL_HISTORYSTACK_H

