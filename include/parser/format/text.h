#ifndef PARSER_FORMAT_TEXT_H
#define PARSER_FORMAT_TEXT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

struct parser_format_Text_Line
{
	char *str;
	uint32_t length;
};

struct parser_format_Text
{
	struct parser_format_Text_Line *array;
	uint32_t length;
	uint32_t alloc;
};

void parser_format_Text_init(void *genericFormat);
void parser_format_Text_deinit(void *genericFormat);
int32_t parser_format_Text_parseFp(void *genericFormat, FILE *fp);
int32_t parser_format_Text_parse(void *genericFormat, const char *filePath);
void parser_format_Text_render(const void *genericFormat);
void parser_format_Text_print(const void *genericFormat);
const char *parser_format_Text_errorMsg(const int32_t genericErrorCode);

#endif // PARSER_FORMAT_TEXT_H

