#include "parser/format/gopher.h"

#include "util/memory.h"
#include <string.h>

static const uint32_t GOPHER_PARSER_EXPAND_BLOCK = 32;

static void
parser_format_Gopher__expand(struct parser_format_Gopher *parser)
{
	if (parser->alloc == 0)
	{
		parser->alloc = GOPHER_PARSER_EXPAND_BLOCK;
		parser->array = util_memory_calloc(
				sizeof(struct parser_format_Gopher_Line), parser->alloc);
	}
	else if (parser->length == (parser->alloc - 1))
	{
		parser->alloc += GOPHER_PARSER_EXPAND_BLOCK;
		parser->array = util_memory_realloc(parser->array,
				sizeof(struct parser_format_Gopher_Line) * parser->alloc);
	}
}

static uint32_t
parser_format_Gopher__lineNextTab(const char *str,
		const uint32_t strSize,
		const uint32_t start)
{
	for (uint32_t i = start; i < strSize; ++i)
	{
		if (str[i] == '\t')
		{
			return i;
		}
	}

	return strSize;
}

static void
parser_format_Gopher__line(struct parser_format_Gopher_Line *line,
		const char *str,
		const uint32_t strSize)
{
	memset(line, 0, sizeof(struct parser_format_Gopher_Line));

	uint32_t nextTab = 0;
	line->type = str[0];
	switch (line->type)
	{
	case '3':	// Error code
		break;
	case '0':	// Text file
	case '1':	// Gopher submenu
	case '4':	// BinHex file
	case '5':	// DOS file
	case '6':	// Unnencoded file
	case '7':	// Full-text search
	case '9':	// Binary file
	case 'g':	// GIF file
	case 'I':	// Image file
	case 'd':	// Doc file
	case 's':	// Sound file
	case 'h':	// HTML file
	{
		uint32_t prevTab = 0;

		nextTab = parser_format_Gopher__lineNextTab(str, strSize, 0);
		strncpy(line->info, str + 1, nextTab);
		prevTab = nextTab;

		nextTab = parser_format_Gopher__lineNextTab(str, strSize, nextTab + 1);
		strncpy(line->selector, str + prevTab + 1, nextTab - prevTab);
		prevTab = nextTab;

		nextTab = parser_format_Gopher__lineNextTab(str, strSize, nextTab + 1);
		strncpy(line->hostname, str + prevTab + 1, nextTab - prevTab);
		prevTab = nextTab;

		strncpy(line->port, str + prevTab + 1, strSize - prevTab - 1);
	}	break;
	case 'i':	// Informational message
		nextTab = parser_format_Gopher__lineNextTab(str, strSize, 0);
		strncpy(line->info, str + 1, nextTab);
		break;
	case '2':	// CCSO Nameserver
	case '8':	// Telnet
	case 'T':	// Telnet 3270
		// Unused
		break;
	default:
		// Unused/unknown
		break;
	}
}

void
parser_format_Gopher_init(void *genericFormat)
{
	struct parser_format_Gopher *parser = genericFormat;
	parser->array = NULL;
	parser->length = 0;
	parser->alloc = 0;
}

void
parser_format_Gopher_deinit(void *genericFormat)
{
	struct parser_format_Gopher *parser = genericFormat;
	if (parser->array != NULL)
	{
		util_memory_free(parser->array);
		parser->array = NULL;
		parser->alloc = 0;
		parser->length = 0;
	}
}

int32_t
parser_format_Gopher_parseFp(void *genericFormat, FILE *fp)
{
	struct parser_format_Gopher *parser = genericFormat;
	rewind(fp);
	char line[1024] = { 0 };

	while (fgets(line, 1024, fp) != NULL)
	{
		parser_format_Gopher__expand(parser);
		parser_format_Gopher__line(&parser->array[parser->length++],
				line,
				strlen(line));
	}
	return 0;
}

int32_t
parser_format_Gopher_parse(void *genericFormat, const char *filePath)
{
	FILE *fp = fopen(filePath, "r");
	const int32_t retVal = parser_format_Gopher_parseFp(genericFormat, fp);
	fclose(fp);
	fp = NULL;
	return retVal;
}

void
parser_format_Gopher_render(const void *genericFormat)
{
	const struct parser_format_Gopher *parser = genericFormat;
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		const struct parser_format_Gopher_Line *line = &parser->array[i];
		switch (line->type)
		{
		case 'i':
			printf("%s\n", line->info);
			break;
		case '0':	// Text file
		case '1':	// Gopher submenu
		case '4':	// BinHex file
		case '5':	// DOS file
		case '6':	// Unnencoded file
		case '7':	// Full-text search
		case '9':	// Binary file
		case 'g':	// GIF file
		case 'I':	// Image file
		case 'd':	// Doc file
		case 's':	// Sound file
		case 'h':	// HTML file
		{
			if (!strncmp(line->selector, "URL:", 4))
			{
				printf("\n[HTML]: %s\n\n", line->info);
			}
			else if (line->type == '1')
			{
				printf("\n[SUBM]: %s\n\n", line->info);
			}
			else
			{
				printf("\n[FILE]: %s\n", line->info);
			}
		}	break;
		default:
			break;
		}
	}
}

void
parser_format_Gopher_print(const void *genericFormat)
{
	const struct parser_format_Gopher *parser = genericFormat;
	(void) parser;
}

const char *
parser_format_Gopher_errorMsg(const int32_t genericErrorCode)
{
	return NULL;
}

