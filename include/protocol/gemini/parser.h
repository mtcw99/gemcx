#ifndef PROTOCOL_GEMINI_PARSER_H
#define PROTOCOL_GEMINI_PARSER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

enum p_gemini_Parser_Type
{
	P_GEMINI_PARSER_TYPE_TEXT = 0,
	P_GEMINI_PARSER_TYPE_LINK,
	P_GEMINI_PARSER_TYPE_HEAD,
	P_GEMINI_PARSER_TYPE_LIST,
	P_GEMINI_PARSER_TYPE_BLOCKQUOTES,
	P_GEMINI_PARSER_TYPE_PREFORMATTED_START,
	P_GEMINI_PARSER_TYPE_PREFORMATTED,
	P_GEMINI_PARSER_TYPE_PREFORMATTED_END,
	P_GEMINI_PARSER_TYPE_LINE,

	P_GEMINI_PARSER_TYPE__TOTAL
};

struct p_gemini_Parser_ContentLink
{
	char *text;
	char *link;

	uint32_t xcbButtonIndex;
};

struct p_gemini_Parser_ContentHead
{
	char *text;
	uint32_t level;
};

struct p_gemini_Parser_Line
{
	enum p_gemini_Parser_Type type;
	union
	{
		char *text;
		struct p_gemini_Parser_ContentLink link;
		struct p_gemini_Parser_ContentHead head;
	} content;
};

enum p_gemini_Parser_Error
{
	GEMINI_PARSER_ERROR_NONE = 0,
	GEMINI_PARSER_ERROR_NO_FILE,

	GEMINI_PARSER_ERROR__TOTAL
};

struct p_gemini_Parser
{
	struct p_gemini_Parser_Line *	array;
	uint32_t 			length;
	uint32_t			alloc;
};

void p_gemini_Parser_init(struct p_gemini_Parser *parser);
void p_gemini_Parser_deinit(struct p_gemini_Parser *parser);
enum p_gemini_Parser_Error p_gemini_Parser_parse(struct p_gemini_Parser *parser,
	const char *fileName);
enum p_gemini_Parser_Error p_gemini_Parser_parseFp(struct p_gemini_Parser *parser,
	FILE *fp);
void p_gemini_Parser_print(const struct p_gemini_Parser *parser);
void p_gemini_Parser_render(const struct p_gemini_Parser *parser);

const char *p_gemini_Parser_errorMsg(const enum p_gemini_Parser_Error error);

#endif // PROTOCOL_GEMINI_PARSER_H

