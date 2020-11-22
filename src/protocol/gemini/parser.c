#include "protocol/gemini/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "util/memory.h"
#include "util/ex.h"

#define PARSER_BUFFER 1024
#define LINE_ALLOC 32

static void
p_gemini_Parser__expand(struct p_gemini_Parser *parser);

static uint32_t
p_gemini_Parser__lineLinkSplit(const char *line, const uint32_t size);

static int32_t
p_gemini_Parser__line(struct p_gemini_Parser_Line *lineContent,
		const char *line,
		const uint32_t size,
		bool *inPreformatted);

static const char *p_gemini_errorMsgs[GEMINI_PARSER_ERROR__TOTAL] = {
	[GEMINI_PARSER_ERROR_NONE] = "",
	[GEMINI_PARSER_ERROR_NO_FILE] = "No files"
};

static const char *p_gemini_typeStrName[P_GEMINI_PARSER_TYPE__TOTAL] = {
	[P_GEMINI_PARSER_TYPE_TEXT] = "Text",
	[P_GEMINI_PARSER_TYPE_LINK] = "Link",
	[P_GEMINI_PARSER_TYPE_HEAD] = "Header",
	[P_GEMINI_PARSER_TYPE_LIST] = "List",
	[P_GEMINI_PARSER_TYPE_BLOCKQUOTES] = "Blockquotes",
	[P_GEMINI_PARSER_TYPE_PREFORMATTED_START] = "Preformatted Start",
	[P_GEMINI_PARSER_TYPE_PREFORMATTED] = "Preformatted",
	[P_GEMINI_PARSER_TYPE_PREFORMATTED_END] = "Preformatted End",
	[P_GEMINI_PARSER_TYPE_LINE] = "Line/Break"
};

void
p_gemini_Parser_init(struct p_gemini_Parser *parser)
{
	parser->alloc = LINE_ALLOC;
	parser->array = util_memory_calloc(sizeof(struct p_gemini_Parser_Line),
			parser->alloc);
	parser->length = 0;
}

void
p_gemini_Parser_deinit(struct p_gemini_Parser *parser)
{
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		struct p_gemini_Parser_Line *line = &parser->array[i];
		switch (line->type)
		{
		case P_GEMINI_PARSER_TYPE_LINK:
			util_memory_free(line->content.link.text);
			util_memory_free(line->content.link.link);
			break;
		case P_GEMINI_PARSER_TYPE_HEAD:
			util_memory_free(line->content.head.text);
			break;
		case P_GEMINI_PARSER_TYPE_LINE:
		case P_GEMINI_PARSER_TYPE_PREFORMATTED_START:
		case P_GEMINI_PARSER_TYPE_PREFORMATTED_END:
			break;
		default:
			if (line->content.text != NULL)
			{
				util_memory_free(line->content.text);
			}
		}
	}

	util_memory_free(parser->array);
	parser->array = NULL;
	parser->length = 0;
	parser->alloc = 0;
} 
static void
p_gemini_Parser__expand(struct p_gemini_Parser *parser)
{
	parser->alloc += LINE_ALLOC;
	parser->array = util_memory_realloc(parser->array,
			sizeof(struct p_gemini_Parser_Line) *
			parser->alloc);
}

static uint32_t
p_gemini_Parser__lineLinkSplit(const char *line, const uint32_t size)
{
	bool alphaNumChar = false;
	for (uint32_t i = 0; i < size; ++i)
	{
		if (alphaNumChar && ((line[i] == ' ') || (line[i] == '\n') || (line[i] == '\t')))
		{
			return i;
		}
		else if (!alphaNumChar && ((line[i] != ' ') && (line[i] != '\n') && (line[i] != '\t')))
		{
			alphaNumChar = true;
		}
	}

	return 0;
}

static int32_t
p_gemini_Parser__line(struct p_gemini_Parser_Line *lineContent,
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
			const uint32_t linkSplit = p_gemini_Parser__lineLinkSplit(
					lineLink, lineLinkSize);
			//printf("linesplit: %d | %s\n", linkSplit, lineLink);

			lineContent->type = P_GEMINI_PARSER_TYPE_LINK;
			lineContent->content.link.link = util_memory_calloc(sizeof(char),
					linkSplit + 1);
			lineContent->content.link.text = util_memory_calloc(sizeof(char),
					lineLinkSize - linkSplit + 1);

			strncpy(lineContent->content.link.link,
					lineLink,
					linkSplit);
			strncpy(lineContent->content.link.text,
					lineLink + linkSplit + 1,
					lineLinkSize - linkSplit - 1);

			// If its empty, then use link as the text
			if (lineContent->content.link.text[0] == '\0')
			{
				lineContent->content.link.text = util_memory_realloc(
						lineContent->content.link.text,
						sizeof(char) * linkSplit + 1);

				strcpy(lineContent->content.link.text,
						lineContent->content.link.link);
			}

			// Clean up links
			util_ex_rmchs(lineContent->content.link.text,
					strlen(lineContent->content.link.text),
					"\t", true);

			util_ex_rmchs(lineContent->content.link.link,
					strlen(lineContent->content.link.link),
					"\t", true);
		}
		break;
	case '#':	// Headers
	{
		lineContent->type = P_GEMINI_PARSER_TYPE_HEAD;
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
		lineContent->content.text = util_memory_calloc(sizeof(char), strlen(line));
		strncpy(lineContent->content.text, line +
				lineContent->content.head.level,
				size - 1 - lineContent->content.head.level);
	}	break;
	case '*':	// List
		lineContent->type = P_GEMINI_PARSER_TYPE_LIST;
		lineContent->content.text = util_memory_calloc(sizeof(char), size);
		strncpy(lineContent->content.text, line + 1, size - 2);
		break;
	case '>':	// Blockquotes
		lineContent->type = P_GEMINI_PARSER_TYPE_BLOCKQUOTES;
		lineContent->content.text = util_memory_calloc(sizeof(char), size);
		strncpy(lineContent->content.text, line + 1, size - 2);
		break;
	case '`':	// Preformatted text
		if (line[1] == '`' && line[2] == '`')
		{
			lineContent->type = (*inPreformatted) ?
				P_GEMINI_PARSER_TYPE_PREFORMATTED_END :
				P_GEMINI_PARSER_TYPE_PREFORMATTED_START;

			*inPreformatted = !*inPreformatted;
		}
		break;
	case '\n':	// Line break
		lineContent->type = P_GEMINI_PARSER_TYPE_LINE;
		break;
	default:	// Normal + Preformatted line
		lineContent->type = (*inPreformatted) ?
				P_GEMINI_PARSER_TYPE_PREFORMATTED :
				P_GEMINI_PARSER_TYPE_TEXT;
		lineContent->content.text = util_memory_calloc(sizeof(char), size);
		strncpy(lineContent->content.text, line, size - 1);
		break;
	}

	return 0;
}

enum p_gemini_Parser_Error
p_gemini_Parser_parseFp(struct p_gemini_Parser *parser,
		FILE *fp)
{
	char buffer[PARSER_BUFFER] = { 0 };

	uint32_t bigBufferAlloc = PARSER_BUFFER * 2;
	uint32_t bigBufferSize = 0;
	char *bigBuffer = util_memory_calloc(sizeof(char), bigBufferAlloc);

	bool inPreformatted = false;

	while (fgets(buffer, PARSER_BUFFER, fp) != NULL)
	{
		const uint32_t lineSize = strlen(buffer);
#if 0
		printf("lineSize: %d | buffer: %s", lineSize, buffer);
#endif

		bigBufferSize += lineSize;
		if (bigBufferSize > bigBufferAlloc)
		{
			bigBufferAlloc += (PARSER_BUFFER * 2);
			bigBuffer = util_memory_realloc(bigBuffer, sizeof(char) * 
					bigBufferAlloc);
		}
		strcat(bigBuffer, buffer);

		if (lineSize != (PARSER_BUFFER - 1))
		{
			if (parser->length == parser->alloc)
			{
				p_gemini_Parser__expand(parser);
			}
			//printf("Line: %s", bigBuffer);
			p_gemini_Parser__line(&parser->array[parser->length],
					bigBuffer,
					bigBufferSize,
					&inPreformatted);
			++parser->length;
			memset(bigBuffer, 0, bigBufferAlloc);
			bigBufferSize = 0;
		}
	}

	util_memory_free(bigBuffer);
	return GEMINI_PARSER_ERROR_NONE;
}

enum p_gemini_Parser_Error
p_gemini_Parser_parse(struct p_gemini_Parser *parser, const char *fileName)
{
	FILE *fp = fopen(fileName, "r");
	if (fp == NULL)
	{
		return GEMINI_PARSER_ERROR_NO_FILE;
	}

	enum p_gemini_Parser_Error errCode = p_gemini_Parser_parseFp(parser, fp);
	fclose(fp);
	return errCode;
}

void
p_gemini_Parser_print(const struct p_gemini_Parser *parser)
{
	printf("Parser lines length: %d\n", parser->length);
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		const struct p_gemini_Parser_Line *line = &parser->array[i];
		printf("(%s) ", p_gemini_typeStrName[line->type]);
		switch (line->type)
		{
		case P_GEMINI_PARSER_TYPE_LINK:
			printf("%s => %s", line->content.link.text,
					line->content.link.link);
			break;
		case P_GEMINI_PARSER_TYPE_HEAD:
			printf("[%d] %s", line->content.head.level,
					line->content.head.text);
			break;
		case P_GEMINI_PARSER_TYPE_LINE:
		case P_GEMINI_PARSER_TYPE_PREFORMATTED_START:
		case P_GEMINI_PARSER_TYPE_PREFORMATTED_END:
			break;
		default:
			printf("%s", line->content.text);
			break;
		}
		putchar('\n');
	}
}

void
p_gemini_Parser_render(const struct p_gemini_Parser *parser)
{
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		const struct p_gemini_Parser_Line *line = &parser->array[i];

		if (line->type == P_GEMINI_PARSER_TYPE_PREFORMATTED_START ||
				line->type == P_GEMINI_PARSER_TYPE_PREFORMATTED_END)
		{
			continue;
		}

		// Prefix
		switch (line->type)
		{
		case P_GEMINI_PARSER_TYPE_LINK:
			printf("-> ");
			break;
		case P_GEMINI_PARSER_TYPE_BLOCKQUOTES:
			printf("| ");
			break;
		case P_GEMINI_PARSER_TYPE_LIST:
			printf(" * ");
			break;
		case P_GEMINI_PARSER_TYPE_HEAD:
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
		case P_GEMINI_PARSER_TYPE_LINK:
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
		case P_GEMINI_PARSER_TYPE_HEAD:
			printf("%s", line->content.head.text);
			break;
		case P_GEMINI_PARSER_TYPE_LINE:
			break;
		default:
			printf("%s", line->content.text);
			break;
		}
		putchar('\n');
	}
}

const char *
p_gemini_Parser_errorMsg(const enum p_gemini_Parser_Error error)
{
	return p_gemini_errorMsgs[error];
}

