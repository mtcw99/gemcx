#include "gopher/parser.h"

#include "util/memory.h"
#include <string.h>

const uint32_t GOPHER_PARSER_EXPAND_BLOCK = 32;

static void
gopher_Parser__expand(struct gopher_Parser *parser)
{
	if (parser->alloc == 0)
	{
		parser->alloc = GOPHER_PARSER_EXPAND_BLOCK;
		parser->array = util_memory_calloc(
				sizeof(struct gopher_Parser_Line), parser->alloc);
	}
	else if (parser->length == (parser->alloc - 1))
	{
		parser->alloc += GOPHER_PARSER_EXPAND_BLOCK;
		parser->array = util_memory_realloc(parser->array,
				sizeof(struct gopher_Parser_Line) * parser->alloc);
	}
}

static uint32_t
gopher_Parser__lineNextTab(const char *str,
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
gopher_Parser__line(struct gopher_Parser_Line *line,
		const char *str,
		const uint32_t strSize)
{
	memset(line, 0, sizeof(struct gopher_Parser_Line));

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

#if 1
		nextTab = gopher_Parser__lineNextTab(str, strSize, 0);
		strncpy(line->info, str + 1, nextTab);
		prevTab = nextTab;

		nextTab = gopher_Parser__lineNextTab(str, strSize, nextTab + 1);
		strncpy(line->selector, str + prevTab + 1, nextTab - prevTab);
		prevTab = nextTab;

		nextTab = gopher_Parser__lineNextTab(str, strSize, nextTab + 1);
		strncpy(line->hostname, str + prevTab + 1, nextTab - prevTab);
		prevTab = nextTab;

		strncpy(line->port, str + prevTab + 1, strSize - prevTab - 1);
#endif
	}	break;
	case 'i':	// Informational message
		nextTab = gopher_Parser__lineNextTab(str, strSize, 0);
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
gopher_Parser_init(struct gopher_Parser *parser)
{
	parser->array = NULL;
	parser->length = 0;
	parser->alloc = 0;
}

void
gopher_Parser_deinit(struct gopher_Parser *parser)
{
	if (parser->alloc)
	{
		util_memory_free(parser->array);
		parser->alloc = 0;
		parser->length = 0;
	}
}

void
gopher_Parser_parseFp(struct gopher_Parser *parser,
		FILE *fp,
		const bool hasHeader)
{
	rewind(fp);
	char line[1024] = { 0 };

	if (hasHeader)
	{
		fgets(line, sizeof(line), fp);
		strcpy(parser->header, line);
	}

	while (fgets(line, 1024, fp) != NULL)
	{
		gopher_Parser__expand(parser);
		gopher_Parser__line(&parser->array[parser->length++],
				line,
				strlen(line));
	}
}

void
gopher_Parser_parse(struct gopher_Parser *parser,
		const char *filePath)
{
	FILE *fp = fopen(filePath, "r");
	gopher_Parser_parseFp(parser, fp, false);
	fclose(fp);
	fp = NULL;
}

void
gopher_Parser_render(const struct gopher_Parser *parser)
{
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		const struct gopher_Parser_Line *line = &parser->array[i];
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

