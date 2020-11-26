#include "parser/format/text.h"

void
parser_format_Text_init(void *genericFormat)
{
}

void
parser_format_Text_deinit(void *genericFormat)
{
}

int32_t
parser_format_Text_parseFp(void *genericFormat, FILE *fp)
{
	return 0;
}

int32_t
parser_format_Text_parse(void *genericFormat, const char *filePath)
{
	return 0;
}

void
parser_format_Text_render(const void *genericFormat)
{
}

void
parser_format_Text_print(const void *genericFormat)
{
}

const char *
parser_format_Text_errorMsg(const int32_t genericErrorCode)
{
	return NULL;
}

