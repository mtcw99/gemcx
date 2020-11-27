#include "render/format/gopher/xcb.h"

#include "util/memory.h"

uint32_t
render_format_gopher_Xcb_render(struct render_Xcb *pgxcb,
		const struct parser_format_Gopher *parser,
		const bool scroll)
{
	static const uint32_t yChange = 20;
	if (!scroll)
	{
		ui_xcb_Pixmap_clear(&pgxcb->pixmap);
	}

	const uint32_t pX = 10;
	uint32_t pY = 10;
	for (uint32_t i = 0; i < parser->length; ++i, pY += yChange)
	{
		const struct parser_format_Gopher_Line *line = &parser->array[i];

		switch (line->type)
		{
		case 'i':	// Informational Text
			if (!scroll)
			{
				ui_xcb_Text_render(&pgxcb->font[4],
						pgxcb->pixmap.pixmap, line->info,
						pX, pY, 0xFFFFFF, 1.0);
			}
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
			pY += 8;
			const int16_t osetPX = pX + pgxcb->offsetX + 30;
			const int16_t osetPY = pY + pgxcb->offsetY;
#if 0
			printf("RENDER px: %d\npy: %d\nindex: %d\nox: %d\noy: %d\nwidth: %d\nheight: %d\n\n",
					pX, pY, line->xcbButtonIndex,
					osetPX, osetPY,
					pgxcb->subwindow->rect.width,
					pgxcb->subwindow->rect.height);
#endif
			if (!scroll)
			{
				ui_xcb_Text_render(&pgxcb->font[4],
						pgxcb->pixmap.pixmap, "►",
						pX, pY, 0xFFFFFF, 1.0);
			}

			render_Links_render(&pgxcb->links,
					line->xcbButtonIndex,
					osetPX, osetPY,
					pgxcb->subwindow->rect.width,
					pgxcb->subwindow->rect.height);

			pY += 5;
		}	break;
		default:
			break;

		}
	} 
	return pY;
}

void
render_format_gopher_Xcb_itemsInit(struct render_Xcb *pgxcb,
		struct parser_format_Gopher *parser)
{
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		struct parser_format_Gopher_Line *line = &parser->array[i];
		switch (line->type)
		{
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
#if 0
			printf("info: %s\tselector: %s\tparentWindow: %d\n",
					line->info, line->selector, pgxcb->parentWindow);
#endif
			line->xcbButtonIndex = render_Links_new(&pgxcb->links,
					line->info, line->selector,
					pgxcb->subwindow->id);
			break;
		default:
			break;
		}
	}
}

