#include "parser/format/unknown.h"

void
parser_format_Unknown_init(void *genericFormat)
{
	struct parser_format_Unknown *unknown = genericFormat;
}

void
parser_format_Unknown_deinit(void *genericFormat)
{
	struct parser_format_Unknown *unknown = genericFormat;
}

int32_t
parser_format_Unknown_parseFp(void *genericFormat, FILE *fp)
{
	struct parser_format_Unknown *unknown = genericFormat;
	return 0;
}

int32_t
parser_format_Unknown_parse(void *genericFormat, const char *filePath)
{
	struct parser_format_Unknown *unknown = genericFormat;
	return 0;
}

void
parser_format_Unknown_render(const void *genericFormat)
{
	const struct parser_format_Unknown *unknown = genericFormat;
}

void
parser_format_Unknown_print(const void *genericFormat)
{
	const struct parser_format_Unknown *unknown = genericFormat;
}

const char *
parser_format_Unknown_errorMsg(const int32_t genericErrorCode)
{
	return NULL;
}

