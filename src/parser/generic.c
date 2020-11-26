#include "parser/generic.h"

static const struct parser_Funcs defFuncs[PARSER_TYPE__TOTAL] = {
	[PARSER_TYPE_UNKNOWN] = {
		.init = parser_format_Unknown_init,
		.deinit = parser_format_Unknown_deinit,
		.parseFp = parser_format_Unknown_parseFp,
		.parse = parser_format_Unknown_parse,
		.render = parser_format_Unknown_render,
		.print = parser_format_Unknown_print,
		.errorMsg = parser_format_Unknown_errorMsg,
	},
	[PARSER_TYPE_GEMINI] = {
		.init = parser_format_Gemini_init,
		.deinit = parser_format_Gemini_deinit,
		.parseFp = parser_format_Gemini_parseFp,
		.parse = parser_format_Gemini_parse,
		.render = parser_format_Gemini_render,
		.print = parser_format_Gemini_print,
		.errorMsg = parser_format_Gemini_errorMsg,
	},
	[PARSER_TYPE_GOPHER] = {
		.init = parser_format_Gopher_init,
		.deinit = parser_format_Gopher_deinit,
		.parseFp = parser_format_Gopher_parseFp,
		.parse = parser_format_Gopher_parse,
		.render = parser_format_Gopher_render,
		.print = parser_format_Gopher_print,
		.errorMsg = parser_format_Gopher_errorMsg,
	},
	[PARSER_TYPE_TEXT] = {
		.init = parser_format_Text_init,
		.deinit = parser_format_Text_deinit,
		.parseFp = parser_format_Text_parseFp,
		.parse = parser_format_Text_parse,
		.render = parser_format_Text_render,
		.print = parser_format_Text_print,
		.errorMsg = parser_format_Text_errorMsg,
	},
	[PARSER_TYPE_IMAGE] = {
		.init = parser_format_Image_init,
		.deinit = parser_format_Image_deinit,
		.parseFp = parser_format_Image_parseFp,
		.parse = parser_format_Image_parse,
		.render = parser_format_Image_render,
		.print = parser_format_Image_print,
		.errorMsg = parser_format_Image_errorMsg,
	},
};

void
Parser_init(struct Parser *parser)
{
	parser->formatList[PARSER_TYPE_UNKNOWN] = &parser->format.unknown;
	parser->formatList[PARSER_TYPE_GEMINI] = &parser->format.gemini;
	parser->formatList[PARSER_TYPE_GOPHER] = &parser->format.gopher;
	parser->formatList[PARSER_TYPE_TEXT] = &parser->format.text;
	parser->formatList[PARSER_TYPE_IMAGE] = &parser->format.image;

	for (uint32_t i = 0; i < PARSER_TYPE__TOTAL; ++i)
	{
		defFuncs[i].init(parser->formatList[i]);
	}
}

void
Parser_deinit(struct Parser *parser)
{
	for (uint32_t i = 0; i < PARSER_TYPE__TOTAL; ++i)
	{
		defFuncs[i].init(parser->formatList[i]);
	}
}

void
Parser_setType(struct Parser *const restrict parser,
		const enum parser_Type type)
{
	parser->type = type;
	parser->func = defFuncs[parser->type];
	parser->focusFormat = parser->formatList[parser->type];
}

int32_t
Parser_parseFp(struct Parser *parser,
		FILE *fp,
		const bool hasHeader)
{
	return parser->func.parseFp(parser->focusFormat, fp);
}

int32_t
Parser_parse(struct Parser *parser,
		const char *filePath)
{
	return parser->func.parse(parser->focusFormat, filePath);
}

void
Parser_render(const struct Parser *parser)
{
	parser->func.render(parser->focusFormat);
}

void
Parser_print(const struct Parser *parser)
{
	parser->func.print(parser->focusFormat);
}

