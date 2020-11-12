#ifndef PROTOCOL_GOPHER_PARSER_H
#define PROTOCOL_GOPHER_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

struct p_gopher_Parser_Line
{
	char type;
	char info[128];
	char selector[128];
	char hostname[128];
	char port[6];
};

struct p_gopher_Parser
{
	struct p_gopher_Parser_Line 	*array;
	uint32_t			length;
	uint32_t			alloc;
	char 				header[128];
};

void p_gopher_Parser_init(struct p_gopher_Parser *parser);
void p_gopher_Parser_deinit(struct p_gopher_Parser *parser);

void p_gopher_Parser_parseFp(struct p_gopher_Parser *parser,
		FILE *fp,
		const bool hasHeader);
void p_gopher_Parser_parse(struct p_gopher_Parser *parser,
		const char *filePath);
void p_gopher_Parser_render(const struct p_gopher_Parser *parser);

#endif // PROTOCOL_GOPHER_PARSER_H

