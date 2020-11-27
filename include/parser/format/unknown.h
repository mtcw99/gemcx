#ifndef PARSER_FORMAT_UNKNOWN_H
#define PARSER_FORMAT_UNKNOWN_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

struct parser_format_Unknown
{
	uint8_t errCode;
};

void parser_format_Unknown_init(void *genericFormat);
void parser_format_Unknown_deinit(void *genericFormat);
int32_t parser_format_Unknown_parseFp(void *genericFormat, FILE *fp);
int32_t parser_format_Unknown_parse(void *genericFormat, const char *filePath);
void parser_format_Unknown_render(const void *genericFormat);
void parser_format_Unknown_print(const void *genericFormat);
const char *parser_format_Unknown_errorMsg(const int32_t genericErrorCode);

#endif // PARSER_FORMAT_UNKNOWN_H

