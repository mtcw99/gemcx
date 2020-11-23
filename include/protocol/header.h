#ifndef PROTOCOL_HEADER_H
#define PROTOCOL_HEADER_H

#include "protocol/type.h"
#include "protocol/gemini/header.h"

struct protocol_Header
{
	enum protocol_Type type;

	union
	{
		struct p_gemini_Header gemini;
	} header;
};

void protocol_Header_getLine(struct protocol_Header *header,
		const char *line);
void protocol_Header_print(const struct protocol_Header *header);
const char *protocol_Header_getStatusCodeStr(const struct protocol_Header *header);
int32_t protocol_Header_getStatusCode(const struct protocol_Header *header);

#endif // PROTOCOL_HEADER_H

