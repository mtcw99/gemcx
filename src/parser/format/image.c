#include "parser/format/image.h"

void
parser_format_Image_init(void *genericFormat)
{
}

void
parser_format_Image_deinit(void *genericFormat)
{
}

int32_t
parser_format_Image_parseFp(void *genericFormat, FILE *fp)
{
	return 0;
}

int32_t
parser_format_Image_parse(void *genericFormat, const char *filePath)
{
	return 0;
}

void
parser_format_Image_render(const void *genericFormat)
{
}

void
parser_format_Image_print(const void *genericFormat)
{
}

const char *
parser_format_Image_errorMsg(const int32_t genericErrorCode)
{
	return NULL;
}

