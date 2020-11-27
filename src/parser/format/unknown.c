#include "parser/format/unknown.h"

void
parser_format_Unknown_init(void *genericFormat)
{
}

void
parser_format_Unknown_deinit(void *genericFormat)
{
}

int32_t
parser_format_Unknown_parseFp(void *genericFormat, FILE *fp)
{
	return 0;
}

int32_t
parser_format_Unknown_parse(void *genericFormat, const char *filePath)
{
	return 0;
}

void
parser_format_Unknown_render(const void *genericFormat)
{
}

void
parser_format_Unknown_print(const void *genericFormat)
{
}

const char *
parser_format_Unknown_errorMsg(const int32_t genericErrorCode)
{
	return NULL;
}

