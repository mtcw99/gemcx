#ifndef GOPHER_PARSER_H
#define GOPHER_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// TODO: Fix gopher parser

struct gopher_Parser_Line
{
	char type;
	char info[128];
	char selector[128];
	char hostname[128];
	char port[6];
};

struct gopher_Parser
{
	struct gopher_Parser_Line 	*array;
	uint32_t			length;
	uint32_t			alloc;
	char 				header[128];
};

void gopher_Parser_init(struct gopher_Parser *parser);
void gopher_Parser_deinit(struct gopher_Parser *parser);

void gopher_Parser_parseFp(struct gopher_Parser *parser,
		FILE *fp,
		const bool hasHeader);
void gopher_Parser_parse(struct gopher_Parser *parser,
		const char *filePath);
void gopher_Parser_render(const struct gopher_Parser *parser);

#endif // GOPHER_PARSER_H

