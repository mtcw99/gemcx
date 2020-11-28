#ifndef PARSER_GENERIC_H
#define PARSER_GENERIC_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "parser/format/unknown.h"
#include "parser/format/gemini.h"
#include "parser/format/gopher.h"
#include "parser/format/text.h"
#include "parser/format/image.h"

enum parser_Type
{
	PARSER_TYPE_UNKNOWN = 0,
	PARSER_TYPE_GEMINI,
	PARSER_TYPE_GOPHER,
	PARSER_TYPE_TEXT,
	PARSER_TYPE_IMAGE,

	PARSER_TYPE__TOTAL
};

#define parser_Type_assert(t) assert(t >= 0 && t < PARSER_TYPE__TOTAL)

struct Parser
{
	enum parser_Type type;

	struct parser_Format
	{
		struct parser_format_Unknown unknown;
		struct parser_format_Gemini gemini;
		struct parser_format_Gopher gopher;
		struct parser_format_Text text;
		struct parser_format_Image image;
	} format;

	void *formatList[PARSER_TYPE__TOTAL];
	void *focusFormat;

	struct parser_Funcs
	{
		void (* init)(void *);
		void (* deinit)(void *);
		int32_t (* parseFp)(void *, FILE *);
		int32_t (* parse)(void *, const char *);
		void (* render)(const void *);
		void (* print)(const void *);
		const char *(* errorMsg)(const int32_t errorCode);
	} func;
};

void Parser_init(struct Parser *parser);
void Parser_deinit(struct Parser *parser);
void Parser_reinit(struct Parser *parser);

void Parser_setType(struct Parser *const restrict parser,
		const enum parser_Type type);

int32_t Parser_parseFp(struct Parser *parser,
		FILE *fp,
		const bool hasHeader);

int32_t Parser_parse(struct Parser *parser,
		const char *filePath);

void Parser_render(const struct Parser *parser);
void Parser_print(const struct Parser *parser);
const char *Parser_errorMsg(const int32_t errorCode);

#endif // PARSER_GENERIC_H

