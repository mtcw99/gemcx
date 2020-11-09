#ifndef GEMINI_PARSER_H
#define GEMINI_PARSER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

enum gemini_Parser_Type
{
	GEMINI_PARSER_TYPE_TEXT = 0,
	GEMINI_PARSER_TYPE_LINK,
	GEMINI_PARSER_TYPE_HEAD,
	GEMINI_PARSER_TYPE_LIST,
	GEMINI_PARSER_TYPE_BLOCKQUOTES,
	GEMINI_PARSER_TYPE_PREFORMATTED_START,
	GEMINI_PARSER_TYPE_PREFORMATTED,
	GEMINI_PARSER_TYPE_PREFORMATTED_END,
	GEMINI_PARSER_TYPE_LINE,

	GEMINI_PARSER_TYPE__TOTAL
};

struct gemini_Parser_ContentLink
{
	char *text;
	char *link;
};

struct gemini_Parser_ContentHead
{
	char *text;
	uint32_t level;
};

struct gemini_Parser_Line
{
	enum gemini_Parser_Type type;
	union
	{
		char *text;
		struct gemini_Parser_ContentLink link;
		struct gemini_Parser_ContentHead head;
	} content;
};

enum gemini_Parser_Error
{
	GEMINI_PARSER_ERROR_NONE = 0,
	GEMINI_PARSER_ERROR_NO_FILE,

	GEMINI_PARSER_ERROR__TOTAL
};

struct gemini_Parser
{
	struct gemini_Parser_Line *	array;
	uint32_t 			length;
	uint32_t			alloc;
};

void gemini_Parser_init(struct gemini_Parser *parser);
void gemini_Parser_deinit(struct gemini_Parser *parser);
enum gemini_Parser_Error gemini_Parser_parse(struct gemini_Parser *parser,
	const char *fileName);
enum gemini_Parser_Error gemini_Parser_parseFp(struct gemini_Parser *parser,
	FILE *fp);
void gemini_Parser_print(const struct gemini_Parser *parser);
void gemini_Parser_render(const struct gemini_Parser *parser);

const char *gemini_Parser_errorMsg(const enum gemini_Parser_Error error);

#endif // GEMINI_PARSER_H

