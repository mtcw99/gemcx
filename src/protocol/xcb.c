#include "protocol/xcb.h"

#include "protocol/gemini/xcb.h"
#include "protocol/gopher/xcb.h"

uint32_t
protocol_Xcb_render(const struct protocol_Parser *parser,
		struct ui_xcb_Pixmap* pixmap,
		struct ui_xcb_Text *text,
		const uint32_t width,
		const uint32_t height,
		const uint32_t yOffset)
{
	switch (parser->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		return p_gemini_Xcb_render(&parser->protocol.gemini, pixmap, text,
				width, height, yOffset);
	case PROTOCOL_TYPE_GOPHER:
		return p_gopher_Xcb_render(&parser->protocol.gopher, pixmap, text);
	default:
		protocol_Type_assert(parser->type);
		break;
	}
	return 0;
}

