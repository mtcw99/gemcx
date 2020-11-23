#include "protocol/header.h"

void
protocol_Header_getLine(struct protocol_Header *header,
		const char *line)
{
	if (header->type == PROTOCOL_TYPE_GEMINI)
	{
		p_gemini_Header_getLine(&header->header.gemini, line);
	}
}

void
protocol_Header_print(const struct protocol_Header *header)
{
	if (header->type == PROTOCOL_TYPE_GEMINI)
	{
		p_gemini_Header_print(&header->header.gemini);
	}
}

const char *
protocol_Header_getStatusCodeStr(const struct protocol_Header *header)
{
	if (header->type == PROTOCOL_TYPE_GEMINI)
	{
		return p_gemini_Header_getStatusCodeStr(&header->header.gemini);
	}
	else
	{
		return NULL;
	}
}

int32_t
protocol_Header_getStatusCode(const struct protocol_Header *header)
{
	if (header->type == PROTOCOL_TYPE_GEMINI)
	{
		return header->header.gemini.statusCodeI32;
	}
	else
	{
		return -1;
	}
}

