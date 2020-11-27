#include "render/xcb.h"

#include "render/format/gemini/xcb.h"
#include "render/format/gopher/xcb.h"

#include "util/memory.h"

enum
{
	BUTTONS_ALLOC_STEP = 16
};

void
render_Xcb_init(struct render_Xcb *pgxcb,
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
	render_Links_init(&pgxcb->links, context, pgxcb->font, backgroundColor);
	pgxcb->offsetX = 0;
	pgxcb->offsetY = 0;
	pgxcb->hasItemsInit = false;

	pgxcb->subwindow = subwindow;
	pgxcb->context = context;
}

void
render_Xcb_deinit(struct render_Xcb *pgxcb)
{
	ui_xcb_Pixmap_deinit(&pgxcb->pixmap);
	render_Links_deinit(&pgxcb->links);
	pgxcb->context = NULL;
	pgxcb->font = NULL;
	pgxcb->subwindow = NULL;
}

void
render_Xcb_itemsInit(struct render_Xcb *pgxcb,
		struct Parser *parser)
{
	if (pgxcb->hasItemsInit)
	{
		render_Links_clear(&pgxcb->links);
	}

	switch (parser->type)
	{
	case PARSER_TYPE_GEMINI:
		render_format_gemini_Xcb_itemsInit(pgxcb, &parser->format.gemini);
		break;
	case PARSER_TYPE_GOPHER:
		render_format_gopher_Xcb_itemsInit(pgxcb, &parser->format.gopher);
		break;
	default:
		parser_Type_assert(parser->type);
		return;
	}
	pgxcb->hasItemsInit = true;
}

uint32_t
render_Xcb_render(struct render_Xcb *pgxcb,
		const struct Parser *parser)
{
	uint32_t retVal = 0;
	switch (parser->type)
	{
	case PARSER_TYPE_GEMINI:
		retVal = render_format_gemini_Xcb_render(pgxcb, &parser->format.gemini, false);
		break;
	case PARSER_TYPE_GOPHER:
		retVal = render_format_gopher_Xcb_render(pgxcb, &parser->format.gopher, false);
		break;
	default:
		parser_Type_assert(parser->type);
		return 0;
	}

	ui_xcb_Pixmap_render(&pgxcb->pixmap, pgxcb->offsetX, pgxcb->offsetY);
	return retVal;
}

void
render_Xcb_scroll(struct render_Xcb *pgxcb,
		const struct Parser *parser)
{
	switch (parser->type)
	{
	case PARSER_TYPE_GEMINI:
		render_format_gemini_Xcb_render(pgxcb, &parser->format.gemini, true);
		break;
	case PARSER_TYPE_GOPHER:
		render_format_gopher_Xcb_render(pgxcb, &parser->format.gopher, true);
		break;
	default:
		parser_Type_assert(parser->type);
		return;
	}
	ui_xcb_Pixmap_render(&pgxcb->pixmap, pgxcb->offsetX, pgxcb->offsetY);
}

void
render_Xcb_offset(struct render_Xcb *pgxcb,
		const uint32_t offsetX, const uint32_t offsetY)
{
	pgxcb->offsetX = offsetX;
	pgxcb->offsetY = offsetY;
}

void
render_Xcb_padding(struct render_Xcb *pgxcb,
		const uint32_t left, const uint32_t right)
{
	pgxcb->paddingLeft = left;
	pgxcb->paddingRight = right;
}

bool
render_Xcb_hoverEnter(struct render_Xcb *pgxcb,
		const xcb_enter_notify_event_t *const restrict enterEv)
{
	if (pgxcb->subwindow->id == enterEv->event)
	{
		return true;
	}

	return render_Links_hoverEnter(&pgxcb->links, enterEv);
}

bool
render_Xcb_hoverLeave(struct render_Xcb *pgxcb,
		const xcb_leave_notify_event_t *const restrict leaveEv)
{
	if (pgxcb->subwindow->id == leaveEv->event)
	{
		return true;
	}

	return render_Links_hoverLeave(&pgxcb->links, leaveEv);
}

