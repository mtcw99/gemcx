#include "parser/format/gemini.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "util/memory.h"
#include "util/ex.h"

enum
{
	PARSER_BUFFER = 1024,
	LINE_ALLOC = 32
};

static const char *parser_format_gemini_errorMsgs[PARSER_FORMAT_GEMINI_ERROR__TOTAL] = {
	[PARSER_FORMAT_GEMINI_ERROR_NONE] = "",
	[PARSER_FORMAT_GEMINI_ERROR_NO_FILE] = "No files"
};

static const char *parser_format_gemini_typeStrName[PARSER_FORMAT_GEMINI_TYPE__TOTAL] = {
	[PARSER_FORMAT_GEMINI_TYPE_TEXT] = "Text",
	[PARSER_FORMAT_GEMINI_TYPE_LINK] = "Link",
	[PARSER_FORMAT_GEMINI_TYPE_HEAD] = "Header",
	[PARSER_FORMAT_GEMINI_TYPE_LIST] = "List",
	[PARSER_FORMAT_GEMINI_TYPE_BLOCKQUOTES] = "Blockquotes",
	[PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED_START] = "Preformatted Start",
	[PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED] = "Preformatted",
	[PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED_END] = "Preformatted End",
	[PARSER_FORMAT_GEMINI_TYPE_LINE] = "Line/Break"
};

static void
parser_format_Gemini__expand(struct parser_format_Gemini *parser)
{
	parser->alloc += LINE_ALLOC;
	parser->array = util_memory_realloc(parser->array,
			sizeof(struct parser_format_Gemini_Line) *
			parser->alloc);
}

static uint32_t
parser_format_Gemini__lineLinkSplit(const char *line, const uint32_t size)
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
parser_format_Gemini__line(struct parser_format_Gemini_Line *lineContent,
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
			const uint32_t linkSplit = parser_format_Gemini__lineLinkSplit(
					lineLink, lineLinkSize);
			//printf("linesplit: %d | %s\n", linkSplit, lineLink);

			lineContent->type = PARSER_FORMAT_GEMINI_TYPE_LINK;
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
		lineContent->type = PARSER_FORMAT_GEMINI_TYPE_HEAD;
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
		lineContent->type = PARSER_FORMAT_GEMINI_TYPE_LIST;
		lineContent->content.text = util_memory_calloc(sizeof(char), size);
		strncpy(lineContent->content.text, line + 1, size - 2);
		break;
	case '>':	// Blockquotes
		lineContent->type = PARSER_FORMAT_GEMINI_TYPE_BLOCKQUOTES;
		lineContent->content.text = util_memory_calloc(sizeof(char), size);
		strncpy(lineContent->content.text, line + 1, size - 2);
		break;
	case '`':	// Preformatted text
		if (line[1] == '`' && line[2] == '`')
		{
			lineContent->type = (*inPreformatted) ?
				PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED_END :
				PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED_START;

			*inPreformatted = !*inPreformatted;
		}
		break;
	case '\n':	// Line break
		lineContent->type = PARSER_FORMAT_GEMINI_TYPE_LINE;
		break;
	default:	// Normal + Preformatted line
		lineContent->type = (*inPreformatted) ?
				PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED :
				PARSER_FORMAT_GEMINI_TYPE_TEXT;
		lineContent->content.text = util_memory_calloc(sizeof(char), size);
		strncpy(lineContent->content.text, line, size - 1);
		break;
	}

	return 0;

}

void
parser_format_Gemini_init(void *genericFormat)
{
	struct parser_format_Gemini *parser = genericFormat;
	parser->alloc = LINE_ALLOC;
	parser->array = util_memory_calloc(sizeof(struct parser_format_Gemini_Line),
			parser->alloc);
	parser->length = 0;
}

void
parser_format_Gemini_deinit(void *genericFormat)
{
	struct parser_format_Gemini *parser = genericFormat;
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		struct parser_format_Gemini_Line *line = &parser->array[i];
		switch (line->type)
		{
		case PARSER_FORMAT_GEMINI_TYPE_LINK:
			util_memory_free(line->content.link.text);
			util_memory_free(line->content.link.link);
			break;
		case PARSER_FORMAT_GEMINI_TYPE_HEAD:
			util_memory_free(line->content.head.text);
			break;
		case PARSER_FORMAT_GEMINI_TYPE_LINE:
		case PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED_START:
		case PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED_END:
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

int32_t
parser_format_Gemini_parseFp(void *genericFormat, FILE *fp)
{
	struct parser_format_Gemini *parser = genericFormat;
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
				parser_format_Gemini__expand(parser);
			}
			//printf("Line: %s", bigBuffer);
			parser_format_Gemini__line(&parser->array[parser->length],
					bigBuffer,
					bigBufferSize,
					&inPreformatted);
			++parser->length;
			memset(bigBuffer, 0, bigBufferAlloc);
			bigBufferSize = 0;
		}
	}

	util_memory_free(bigBuffer);
	return PARSER_FORMAT_GEMINI_ERROR_NONE;
}

int32_t
parser_format_Gemini_parse(void *genericFormat, const char *filePath)
{
	FILE *fp = fopen(filePath, "r");
	if (fp == NULL)
	{
		return PARSER_FORMAT_GEMINI_ERROR_NO_FILE;
	}

	enum parser_format_Gemini_Error errCode = parser_format_Gemini_parseFp(genericFormat, fp);
	fclose(fp);
	return errCode;
}

void
parser_format_Gemini_render(const void *genericFormat)
{
	const struct parser_format_Gemini *parser = genericFormat;
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		const struct parser_format_Gemini_Line *line = &parser->array[i];

		if (line->type == PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED_START ||
				line->type == PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED_END)
		{
			continue;
		}

		// Prefix
		switch (line->type)
		{
		case PARSER_FORMAT_GEMINI_TYPE_LINK:
			printf("-> ");
			break;
		case PARSER_FORMAT_GEMINI_TYPE_BLOCKQUOTES:
			printf("| ");
			break;
		case PARSER_FORMAT_GEMINI_TYPE_LIST:
			printf(" * ");
			break;
		case PARSER_FORMAT_GEMINI_TYPE_HEAD:
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
		case PARSER_FORMAT_GEMINI_TYPE_LINK:
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
		case PARSER_FORMAT_GEMINI_TYPE_HEAD:
			printf("%s", line->content.head.text);
			break;
		case PARSER_FORMAT_GEMINI_TYPE_LINE:
			break;
		default:
			printf("%s", line->content.text);
			break;
		}
		putchar('\n');
	}

}

void
parser_format_Gemini_print(const void *genericFormat)
{
	const struct parser_format_Gemini *parser = genericFormat;
	printf("Parser lines length: %d\n", parser->length);
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		const struct parser_format_Gemini_Line *line = &parser->array[i];
		printf("(%s) ", parser_format_gemini_typeStrName[line->type]);
		switch (line->type)
		{
		case PARSER_FORMAT_GEMINI_TYPE_LINK:
			printf("%s => %s", line->content.link.text,
					line->content.link.link);
			break;
		case PARSER_FORMAT_GEMINI_TYPE_HEAD:
			printf("[%d] %s", line->content.head.level,
					line->content.head.text);
			break;
		case PARSER_FORMAT_GEMINI_TYPE_LINE:
		case PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED_START:
		case PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED_END:
			break;
		default:
			printf("%s", line->content.text);
			break;
		}
		putchar('\n');
	}
}

const char *
parser_format_Gemini_errorMsg(const int32_t genericErrorCode)
{
	return parser_format_gemini_errorMsgs[genericErrorCode];
}

