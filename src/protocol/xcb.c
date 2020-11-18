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
		struct ui_xcb_Subwindow *subwindow,
		const uint32_t width,
		const uint32_t height,
		const uint32_t backgroundColor)
{
	pgxcb->font = font;

	ui_xcb_Pixmap_init(&pgxcb->pixmap, context, drawable, width, height,
			backgroundColor);
	protocol_Links_init(&pgxcb->links, context, pgxcb->font, backgroundColor);
	pgxcb->offsetX = 0;
	pgxcb->offsetY = 0;
	pgxcb->hasItemsInit = false;

	pgxcb->subwindow = subwindow;
	pgxcb->context = context;
}

void
protocol_Xcb_deinit(struct protocol_Xcb *pgxcb)
{
	ui_xcb_Pixmap_deinit(&pgxcb->pixmap);
	protocol_Links_deinit(&pgxcb->links);
	pgxcb->context = NULL;
	pgxcb->font = NULL;
	pgxcb->subwindow = NULL;
}

void
protocol_Xcb_itemsInit(struct protocol_Xcb *pgxcb,
		struct protocol_Parser *parser)
{
	if (pgxcb->hasItemsInit)
	{
		protocol_Links_clear(&pgxcb->links);
	}

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
		return;
	}
	pgxcb->hasItemsInit = true;
}

uint32_t
protocol_Xcb_render(struct protocol_Xcb *pgxcb,
		const struct protocol_Parser *parser)
{
	uint32_t retVal = 0;
	switch (parser->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		retVal = p_gemini_Xcb_render(pgxcb, &parser->protocol.gemini, false);
		break;
	case PROTOCOL_TYPE_GOPHER:
		retVal = p_gopher_Xcb_render(pgxcb, &parser->protocol.gopher, false);
		break;
	default:
		protocol_Type_assert(parser->type);
		return 0;
	}

	ui_xcb_Pixmap_render(&pgxcb->pixmap, pgxcb->offsetX, pgxcb->offsetY);
	return retVal;
}

void
protocol_Xcb_scroll(struct protocol_Xcb *pgxcb,
		const struct protocol_Parser *parser)
{
	switch (parser->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		p_gemini_Xcb_render(pgxcb, &parser->protocol.gemini, true);
		break;
	case PROTOCOL_TYPE_GOPHER:
		p_gopher_Xcb_render(pgxcb, &parser->protocol.gopher, true);
		break;
	default:
		protocol_Type_assert(parser->type);
		return;
	}
	ui_xcb_Pixmap_render(&pgxcb->pixmap, pgxcb->offsetX, pgxcb->offsetY);
}

void
protocol_Xcb_offset(struct protocol_Xcb *pgxcb,
		const uint32_t offsetX, const uint32_t offsetY)
{
	pgxcb->offsetX = offsetX;
	pgxcb->offsetY = offsetY;
}

void
protocol_Xcb_padding(struct protocol_Xcb *pgxcb,
		const uint32_t left, const uint32_t right)
{
	pgxcb->paddingLeft = left;
	pgxcb->paddingRight = right;
}

bool
protocol_Xcb_hoverEnter(struct protocol_Xcb *pgxcb,
		const xcb_enter_notify_event_t *const restrict enterEv)
{
	if (pgxcb->subwindow->id == enterEv->event)
	{
		return true;
	}

	return protocol_Links_hoverEnter(&pgxcb->links, enterEv);
}

bool
protocol_Xcb_hoverLeave(struct protocol_Xcb *pgxcb,
		const xcb_leave_notify_event_t *const restrict leaveEv)
{
	if (pgxcb->subwindow->id == leaveEv->event)
	{
		return true;
	}

	return protocol_Links_hoverLeave(&pgxcb->links, leaveEv);
}

