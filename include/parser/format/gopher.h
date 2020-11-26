#ifndef PARSER_FORMAT_GOPHER_H
#define PARSER_FORMAT_GOPHER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

struct parser_format_Gopher_Line
{
	char type;
	char info[128];
	char selector[128];
	char hostname[128];
	char port[6];

	uint32_t xcbButtonIndex;
};

struct parser_format_Gopher
{
	struct parser_format_Gopher_Line *array;
	uint32_t			length;
	uint32_t			alloc;
	char 				header[128];
};

void parser_format_Gopher_init(void *genericFormat);
void parser_format_Gopher_deinit(void *genericFormat);
int32_t parser_format_Gopher_parseFp(void *genericFormat, FILE *fp);
int32_t parser_format_Gopher_parse(void *genericFormat, const char *filePath);
void parser_format_Gopher_render(const void *genericFormat);
void parser_format_Gopher_print(const void *genericFormat);
const char *parser_format_Gopher_errorMsg(const int32_t genericErrorCode);

#endif // PARSER_FORMAT_GOPHER_H

