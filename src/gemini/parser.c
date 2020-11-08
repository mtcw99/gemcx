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

static const char *gemini_typeStrName[GEMINI_PARSER_TYPE__TOTAL] = {
	[GEMINI_PARSER_TYPE_TEXT] = "Text",
	[GEMINI_PARSER_TYPE_LINK] = "Link",
	[GEMINI_PARSER_TYPE_HEAD] = "Header",
	[GEMINI_PARSER_TYPE_LIST] = "List",
	[GEMINI_PARSER_TYPE_BLOCKQUOTES] = "Blockquotes",
	[GEMINI_PARSER_TYPE_PREFORMATTED_START] = "Preformatted Start",
	[GEMINI_PARSER_TYPE_PREFORMATTED] = "Preformatted",
	[GEMINI_PARSER_TYPE_PREFORMATTED_END] = "Preformatted End",
	[GEMINI_PARSER_TYPE_LINE] = "Line/Break"
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
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		struct gemini_Parser_Line *line = &parser->array[i];
		switch (line->type)
		{
		case GEMINI_PARSER_TYPE_LINK:
			free(line->content.link.text);
			free(line->content.link.link);
			break;
		case GEMINI_PARSER_TYPE_HEAD:
			free(line->content.head.text);
			break;
		default:
			free(line->content.text);
		}
	}

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
			lineContent->content.link.link = calloc(sizeof(char),
					linkSplit + 1);
			lineContent->content.link.text = calloc(sizeof(char),
					lineLinkSize - linkSplit + 1);

			strncpy(lineContent->content.link.link,
					lineLink,
					linkSplit);
			strncpy(lineContent->content.link.text,
					lineLink + linkSplit,
					lineLinkSize - linkSplit - 1);
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
		strncpy(lineContent->content.text, line +
				lineContent->content.head.level,
				size - 1 - lineContent->content.head.level);
	}	break;
	case '*':	// List
		lineContent->type = GEMINI_PARSER_TYPE_LIST;
		lineContent->content.text = calloc(sizeof(char), size);
		strncpy(lineContent->content.text, line + 1, size - 2);
		break;
	case '>':	// Blockquotes
		lineContent->type = GEMINI_PARSER_TYPE_BLOCKQUOTES;
		lineContent->content.text = calloc(sizeof(char), size);
		strncpy(lineContent->content.text, line + 1, size - 2);
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
	case '\n':	// Line break
		lineContent->type = GEMINI_PARSER_TYPE_LINE;
		break;
	default:	// Normal + Preformatted line
		lineContent->type = (*inPreformatted) ?
				GEMINI_PARSER_TYPE_PREFORMATTED :
				GEMINI_PARSER_TYPE_TEXT;
		lineContent->content.text = calloc(sizeof(char), size);
		strncpy(lineContent->content.text, line, size - 1);
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

	uint32_t bigBufferAlloc = PARSER_BUFFER * 2;
	uint32_t bigBufferSize = 0;
	char *bigBuffer = calloc(sizeof(char), bigBufferAlloc);

	bool inPreformatted = false;

	while (fgets(buffer, PARSER_BUFFER, fp) != NULL)
	{
		const uint32_t lineSize = strlen(buffer);
		bigBufferSize += lineSize;
		if (bigBufferSize > bigBufferAlloc)
		{
			bigBufferAlloc += (PARSER_BUFFER * 2);
			bigBuffer = realloc(bigBuffer, sizeof(char) * 
					bigBufferAlloc);
		}
		strcat(bigBuffer, buffer);

		if (lineSize != (PARSER_BUFFER - 1))
		{
			if (parser->length == parser->alloc)
			{
				gemini_Parser__expand(parser);
			}
			gemini_Parser__line(&parser->array[parser->length],
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

void
gemini_Parser_print(const struct gemini_Parser *parser)
{
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		const struct gemini_Parser_Line *line = &parser->array[i];
		printf("(%s) ", gemini_typeStrName[line->type]);
		switch (line->type)
		{
		case GEMINI_PARSER_TYPE_LINK:
			printf("%s => %s", line->content.link.text,
					line->content.link.link);
			break;
		case GEMINI_PARSER_TYPE_HEAD:
			printf("[%d] %s", line->content.head.level,
					line->content.head.text);
			break;
		case GEMINI_PARSER_TYPE_LINE:
		case GEMINI_PARSER_TYPE_PREFORMATTED_START:
		case GEMINI_PARSER_TYPE_PREFORMATTED_END:
			break;
		default:
			printf("%s", line->content.text);
			break;
		}
		putchar('\n');
	}
}

void
gemini_Parser_render(const struct gemini_Parser *parser)
{
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		const struct gemini_Parser_Line *line = &parser->array[i];

		if (line->type == GEMINI_PARSER_TYPE_PREFORMATTED_START ||
				line->type == GEMINI_PARSER_TYPE_PREFORMATTED_END)
		{
			continue;
		}

		// Prefix
		switch (line->type)
		{
		case GEMINI_PARSER_TYPE_LINK:
			printf("-> ");
			break;
		case GEMINI_PARSER_TYPE_BLOCKQUOTES:
			printf("| ");
			break;
		case GEMINI_PARSER_TYPE_LIST:
			printf(" * ");
			break;
		case GEMINI_PARSER_TYPE_HEAD:
			for (uint32_t h = 0; h < line->content.head.level; ++h)
			{
				putchar('#');
			}
			putchar(' ');
			break;
		default:
			break;
		}

		// Content
		switch (line->type)
		{
		case GEMINI_PARSER_TYPE_LINK:
			if (line->content.link.text[0] == '\0')
			{
				printf("%s", line->content.link.link);
			}
			else
			{
				printf("%s (%s)", line->content.link.text,
						line->content.link.link);
			}
			break;
		case GEMINI_PARSER_TYPE_HEAD:
			printf("%s", line->content.head.text);
			break;
		case GEMINI_PARSER_TYPE_LINE:
			break;
		default:
			printf("%s", line->content.text);
			break;
		}
		putchar('\n');
	}
}

const char *
gemini_Parser_errorMsg(const enum gemini_Parser_Error error)
{
	return gemini_errorMsgs[error];
}

