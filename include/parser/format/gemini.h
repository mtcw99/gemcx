#ifndef PARSER_FORMAT_GEMINI_H
#define PARSER_FORMAT_GEMINI_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

enum parser_format_Gemini_Type
{
	PARSER_FORMAT_GEMINI_TYPE_TEXT = 0,
	PARSER_FORMAT_GEMINI_TYPE_LINK,
	PARSER_FORMAT_GEMINI_TYPE_HEAD,
	PARSER_FORMAT_GEMINI_TYPE_LIST,
	PARSER_FORMAT_GEMINI_TYPE_BLOCKQUOTES,
	PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED_START,
	PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED,
	PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED_END,
	PARSER_FORMAT_GEMINI_TYPE_LINE,

	PARSER_FORMAT_GEMINI_TYPE__TOTAL
};

struct parser_format_Gemini_Line
{
	enum parser_format_Gemini_Type type;

	union
	{
		char *text;

		struct parser_format_Gemini_ContentLink
		{
			char *text;
			char *link;

			uint32_t xcbButtonIndex;
		} link;

		struct parser_format_Gemini_ContentHead
		{
			char *text;
			uint32_t level;
		} head;
	} content;
};

enum parser_format_Gemini_Error
{
	PARSER_FORMAT_GEMINI_ERROR_NONE = 0,
	PARSER_FORMAT_GEMINI_ERROR_NO_FILE,

	PARSER_FORMAT_GEMINI_ERROR__TOTAL
};

struct parser_format_Gemini
{
	struct parser_format_Gemini_Line *array;
	uint32_t 			length;
	uint32_t			alloc;
};

void parser_format_Gemini_init(void *genericFormat);
void parser_format_Gemini_deinit(void *genericFormat);
int32_t parser_format_Gemini_parseFp(void *genericFormat, FILE *fp);
int32_t parser_format_Gemini_parse(void *genericFormat, const char *filePath);
void parser_format_Gemini_render(const void *genericFormat);
void parser_format_Gemini_print(const void *genericFormat);
const char *parser_format_Gemini_errorMsg(const int32_t genericErrorCode);

#endif // PARSER_FORMAT_GEMINI_H

