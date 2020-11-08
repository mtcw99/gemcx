#include "gemini/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PARSER_BUFFER 1024
#define LINE_ALLOC 32

static void
gemini_Parser__expand(struct gemini_Parser *parser);

static uint32_t
gemini_Parser__lineLinkSplit(const char *line, const uint32_t size);

static int32_t
gemini_Parser__line(struct gemini_Parser_Line *lineContent,
		const char *line,
		const uint32_t size,
		bool *inPreformatted);

static const char *gemini_errorMsgs[GEMINI_PARSER_ERROR__TOTAL] = {
	[GEMINI_PARSER_ERROR_NONE] = "",
	[GEMINI_PARSER_ERROR_NO_FILE] = "No files"
};

void
gemini_Parser_init(struct gemini_Parser *parser)
{
	parser->alloc = LINE_ALLOC;
	parser->array = calloc(sizeof(struct gemini_Parser_Line),
			parser->alloc);
	parser->length = 0;
}

void
gemini_Parser_deinit(struct gemini_Parser *parser)
{
	free(parser->array);
	parser->length = 0;
	parser->alloc = 0;
}

static void
gemini_Parser__expand(struct gemini_Parser *parser)
{
	parser->alloc += LINE_ALLOC;
	parser->array = calloc(sizeof(struct gemini_Parser_Line),
			parser->alloc);
}

static uint32_t
gemini_Parser__lineLinkSplit(const char *line, const uint32_t size)
{
	bool alphaNumChar = false;
	for (uint32_t i = 0; i < size; ++i)
	{
		if (!alphaNumChar && (isalnum(line[i]) || ispunct(line[i])))
		{
			alphaNumChar = true;
		}
		else if (alphaNumChar && ((line[i] == ' ') || (line[i] == '\n')))
		{
			return i;
		}
	}

	return 0;
}

static int32_t
gemini_Parser__line(struct gemini_Parser_Line *lineContent,
		const char *line,
		const uint32_t size,
		bool *inPreformatted)
{
	switch (line[0])
	{
	case '=':	// Link
		if (line[1] == '>')
		{
			const char *lineLink = line + 2;
			const uint32_t lineLinkSize = size - 2;
			const uint32_t linkSplit = gemini_Parser__lineLinkSplit(
					lineLink, lineLinkSize);

			lineContent->type = GEMINI_PARSER_TYPE_LINK;
			strncpy(lineContent->content.link.link, lineLink, linkSplit);
			strcpy(lineContent->content.link.text, lineLink + linkSplit);
		}
		break;
	case '#':	// Headers
	{
		lineContent->type = GEMINI_PARSER_TYPE_HEAD;
		if (line[1] == '#' && line[2] == '#')
		{
			lineContent->content.head.level = 3;
		}
		else if (line[1] == '#')
		{
			lineContent->content.head.level = 2;
		}
		else
		{
			lineContent->content.head.level = 1;
		}
		lineContent->content.text = calloc(sizeof(char), strlen(line));
		strcpy(lineContent->content.text, line +
				lineContent->content.head.level);
	}	break;
	case '*':	// List
		lineContent->type = GEMINI_PARSER_TYPE_LIST;
		lineContent->content.text = calloc(sizeof(char), size);
		strcpy(lineContent->content.text, line + 1);
		break;
	case '>':	// Blockquotes
		lineContent->type = GEMINI_PARSER_TYPE_BLOCKQUOTES;
		lineContent->content.text = calloc(sizeof(char), size);
		strcpy(lineContent->content.text, line + 1);
		break;
	case '`':	// Preformatted text
		if (line[1] == '`' && line[2] == '`')
		{
			lineContent->type = (*inPreformatted) ?
				GEMINI_PARSER_TYPE_PREFORMATTED_END :
				GEMINI_PARSER_TYPE_PREFORMATTED_START;

			*inPreformatted = !*inPreformatted;
		}
		break;
	case '\n':
		lineContent->type = GEMINI_PARSER_TYPE_LINE;
		break;
	default:
		if (*inPreformatted)
		{
			lineContent->type = GEMINI_PARSER_TYPE_PREFORMATTED;
		}
		else
		{
			lineContent->type = GEMINI_PARSER_TYPE_TEXT;
		}
		break;
	}

	return 0;
}

enum gemini_Parser_Error
gemini_Parser_parse(struct gemini_Parser *parser, const char *fileName)
{
	FILE *fp = fopen(fileName, "r");
	if (fp == NULL)
	{
		return GEMINI_PARSER_ERROR_NO_FILE;
	}

	char buffer[PARSER_BUFFER] = { 0 };
	char *bigBuffer = calloc(sizeof(char), PARSER_BUFFER * 2);
	uint32_t bigBufferAlloc = PARSER_BUFFER * 2;
	uint32_t bigBufferSize = 0;
	bool inPreformatted = false;

	while (fgets(buffer, PARSER_BUFFER, fp) != NULL)
	{
		const uint32_t lineSize = strlen(buffer);
		strcat(bigBuffer, buffer);
		bigBufferSize += lineSize;

		if (lineSize != PARSER_BUFFER)
		{
			gemini_Parser__line(&parser->array[parser->length - 1],
					bigBuffer,
					bigBufferSize,
					&inPreformatted);
			++parser->length;
			memset(bigBuffer, 0, bigBufferAlloc);
			bigBufferSize = 0;
		}
	}

	free(bigBuffer);
	fclose(fp);
	return GEMINI_PARSER_ERROR_NONE;
}

const char *
gemini_Parser_errorMsg(const enum gemini_Parser_Error error)
{
	return gemini_errorMsgs[error];
}

