#include "protocol/xcb.h"

#include "protocol/gemini/xcb.h"
#include "protocol/gopher/xcb.h"

#include "util/memory.h"

enum
{
	BUTTONS_ALLOC_STEP = 16
};

void
protocol_Xcb_init(struct protocol_Xcb *pgxcb,
		struct ui_xcb_Context *context,
		struct ui_xcb_Text *font,
		const xcb_drawable_t drawable,
		const xcb_window_t parentWindow,
		const uint32_t width,
		const uint32_t height,
		const uint32_t backgroundColor,
		const enum protocol_Type type)
{
	pgxcb->type = type;
	ui_xcb_Pixmap_init(&pgxcb->pixmap, context, drawable, width, height,
			backgroundColor);
	protocol_Links_init(&pgxcb->links, context, font, backgroundColor);
	pgxcb->offsetX = 0;
	pgxcb->offsetY = 0;
	pgxcb->parentWindow = parentWindow;

	pgxcb->context = context;
	pgxcb->font = font;
}

void
protocol_Xcb_deinit(struct protocol_Xcb *pgxcb)
{
	ui_xcb_Pixmap_deinit(&pgxcb->pixmap);
	protocol_Links_deinit(&pgxcb->links);
	pgxcb->context = NULL;
	pgxcb->font = NULL;
}

void
protocol_Xcb_itemsInit(struct protocol_Xcb *pgxcb,
		struct protocol_Parser *parser)
{
	switch (parser->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		p_gemini_Xcb_itemsInit(pgxcb, &parser->protocol.gemini);
		break;
	case PROTOCOL_TYPE_GOPHER:
		p_gopher_Xcb_itemsInit(pgxcb, &parser->protocol.gopher);
		break;
	default:
		protocol_Type_assert(parser->type);
		break;
	}
}

uint32_t
protocol_Xcb_render(struct protocol_Xcb *pgxcb,
		const struct protocol_Parser *parser,
		const uint32_t width,
		const uint32_t height)
{
	uint32_t retVal = 0;
	switch (parser->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		retVal = p_gemini_Xcb_render(pgxcb, &parser->protocol.gemini, width, height);
		break;
	case PROTOCOL_TYPE_GOPHER:
		retVal = p_gopher_Xcb_render(pgxcb, &parser->protocol.gopher);
		break;
	default:
		protocol_Type_assert(parser->type);
		return 0;
	}

	ui_xcb_Pixmap_render(&pgxcb->pixmap, pgxcb->offsetX, pgxcb->offsetY);
	return retVal;
}

void
protocol_Xcb_offset(struct protocol_Xcb *pgxcb,
		const uint32_t offsetX, const uint32_t offsetY)
{
	pgxcb->offsetX = offsetX;
	pgxcb->offsetY = offsetY;
}

