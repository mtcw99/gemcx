#ifndef PROTOCOL_PARSER_H
#define PROTOCOL_PARSER_H

#include <stdint.h>
#include <stdbool.h>

#include "protocol/type.h"
#include "protocol/gemini/parser.h"
#include "protocol/gopher/parser.h"

struct protocol_Parser
{
	enum protocol_Type type;

	struct
	{
		struct p_gemini_Parser gemini;
		struct p_gopher_Parser gopher;
	} protocol;
};

void protocol_Parser_init(struct protocol_Parser *parser);
void protocol_Parser_deinit(struct protocol_Parser *parser);

void protocol_Parser_setType(struct protocol_Parser *const restrict parser,
		const enum protocol_Type type);

void protocol_Parser_parseFp(struct protocol_Parser *parser,
		FILE *fp,
		const bool hasHeader);
void protocol_Parser_parse(struct protocol_Parser *parser,
		const char *filePath);
void protocol_Parser_render(const struct protocol_Parser *parser);
void protocol_Parser_print(const struct protocol_Parser *parser);

#endif // PROTOCOL_PARSER_H

