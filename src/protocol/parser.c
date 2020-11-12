#include "protocol/parser.h"

void
protocol_Parser_init(struct protocol_Parser *parser,
		const enum protocol_Type type)
{
	parser->type = type;

	switch (parser->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		p_gemini_Parser_init(&parser->protocol.gemini);
		break;
	case PROTOCOL_TYPE_GOPHER:
		p_gopher_Parser_init(&parser->protocol.gopher);
		break;
	default:
		protocol_Type_assert(parser->type);
		break;
	}
}

void
protocol_Parser_deinit(struct protocol_Parser *parser)
{
	switch (parser->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		p_gemini_Parser_deinit(&parser->protocol.gemini);
		break;
	case PROTOCOL_TYPE_GOPHER:
		p_gopher_Parser_deinit(&parser->protocol.gopher);
		break;
	default:
		protocol_Type_assert(parser->type);
		break;
	}
}

void
protocol_Parser_parseFp(struct protocol_Parser *parser,
		FILE *fp,
		const bool hasHeader)
{
	switch (parser->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		p_gemini_Parser_parseFp(&parser->protocol.gemini, fp);
		break;
	case PROTOCOL_TYPE_GOPHER:
		p_gopher_Parser_parseFp(&parser->protocol.gopher, fp, hasHeader);
		break;
	default:
		protocol_Type_assert(parser->type);
		break;
	}
}

void
protocol_Parser_parse(struct protocol_Parser *parser,
		const char *filePath)
{
	switch (parser->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		p_gemini_Parser_parse(&parser->protocol.gemini, filePath);
		break;
	case PROTOCOL_TYPE_GOPHER:
		p_gopher_Parser_parse(&parser->protocol.gopher, filePath);
		break;
	default:
		protocol_Type_assert(parser->type);
		break;
	}
}

void
protocol_Parser_render(const struct protocol_Parser *parser)
{
	switch (parser->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		p_gemini_Parser_render(&parser->protocol.gemini);
		break;
	case PROTOCOL_TYPE_GOPHER:
		p_gopher_Parser_render(&parser->protocol.gopher);
		break;
	default:
		protocol_Type_assert(parser->type);
		break;
	}
}

