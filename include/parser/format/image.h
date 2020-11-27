#ifndef PARSER_FORMAT_IMAGE_H
#define PARSER_FORMAT_IMAGE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

struct parser_format_Image
{
	char fileName[1024];
};

void parser_format_Image_init(void *genericFormat);
void parser_format_Image_deinit(void *genericFormat);
int32_t parser_format_Image_parseFp(void *genericFormat, FILE *fp);
int32_t parser_format_Image_parse(void *genericFormat, const char *filePath);
void parser_format_Image_render(const void *genericFormat);
void parser_format_Image_print(const void *genericFormat);
const char *parser_format_Image_errorMsg(const int32_t genericErrorCode);

#endif // PARSER_FORMAT_IMAGE_H

