#include "parser/format/text.h"

#include "util/memory.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum
{
	PARSER_FORMAT_TEXT_EXPAND = 64
};

static void
parser_format_Text__expand(struct parser_format_Text *fText)
{
	if (fText->array == NULL || fText->alloc == 0)
	{
		fText->alloc = PARSER_FORMAT_TEXT_EXPAND;
		fText->array = util_memory_calloc(
				sizeof(struct parser_format_Text_Line), fText->alloc);
		fText->length = 0;
	}
	else if (fText->length == (fText->alloc - 1))
	{
		fText->alloc += PARSER_FORMAT_TEXT_EXPAND;
		fText->array = util_memory_realloc(fText->array,
				sizeof(struct parser_format_Text_Line) * fText->alloc);
	}
}

void
parser_format_Text_init(void *genericFormat)
{
	struct parser_format_Text *fText = genericFormat;
	fText->array = NULL;
	fText->length = 0;
	fText->alloc = 0;
	fText->rendered = false;
}

void
parser_format_Text_deinit(void *genericFormat)
{
	struct parser_format_Text *fText = genericFormat;
	if (fText->array != NULL)
	{
		for (uint32_t i = 0; i < fText->length; ++i)
		{
			util_memory_free(fText->array[i].str);
			fText->array[i].str = NULL;
			fText->array[i].length = 0;
		}
		util_memory_free(fText->array);
		fText->array = NULL;
	}
	fText->length = 0;
	fText->alloc = 0;
	fText->rendered = false;
}

static inline void
parser_format_Text__line(struct parser_format_Text_Line *fTextLine,
		const char *line,
		const uint32_t lineSize)
{
	fTextLine->str = util_memory_calloc(sizeof(char), lineSize + 1);
	strcpy(fTextLine->str, line);
	fTextLine->length = lineSize;
}

int32_t
parser_format_Text_parseFp(void *genericFormat, FILE *fp)
{
	struct parser_format_Text *fText = genericFormat;
	rewind(fp);

	char line[1024] = { 0 };
	uint32_t bigLineAlloc = 1024 * 2;
	uint32_t bigLineSize = 0;
	char *bigLine = util_memory_calloc(sizeof(char), bigLineAlloc);

	while (fgets(line, 1024, fp) != NULL)
	{
		const uint32_t lineSize = strlen(line);

		bigLineSize += lineSize;
		if (bigLineSize >= bigLineAlloc)
		{
			bigLineAlloc += 1024 * 2;
			bigLine = util_memory_realloc(bigLine,
					sizeof(char) * bigLineAlloc);
		}
		strcat(bigLine, line);

		if (lineSize < 1023)
		{
			parser_format_Text__expand(fText);
			parser_format_Text__line(&fText->array[fText->length],
					bigLine,
					bigLineSize);
			++fText->length;
			memset(bigLine, 0, bigLineAlloc);
			bigLineSize = 0;
		}
	}

	util_memory_free(bigLine);
	fText->rendered = false;
	return 0;
}

int32_t
parser_format_Text_parse(void *genericFormat, const char *filePath)
{
	FILE *fp = fopen(filePath, "r");
	parser_format_Text_parseFp(genericFormat, fp);
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return 0;
}

void
parser_format_Text_render(const void *genericFormat)
{
}

void
parser_format_Text_print(const void *genericFormat)
{
}

const char *
parser_format_Text_errorMsg(const int32_t genericErrorCode)
{
	return NULL;
}

