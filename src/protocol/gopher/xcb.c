#include "protocol/gopher/xcb.h"

#include "util/memory.h"

uint32_t
p_gopher_Xcb_render(struct protocol_Xcb *pgxcb,
		const struct p_gopher_Parser *parser)
{
	static const uint32_t yChange = 20;
	ui_xcb_Pixmap_clear(&pgxcb->pixmap);

	const uint32_t pX = 10;
	uint32_t pY = 10;
	for (uint32_t i = 0; i < parser->length; ++i, pY += yChange)
	{
		const struct p_gopher_Parser_Line *line = &parser->array[i];

		switch (line->type)
		{
		case 'i':	// Informational Text
			ui_xcb_Text_render(pgxcb->font,
					pgxcb->pixmap.pixmap, line->info,
					pX, pY, 0xFFFFFF, 1.0);
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
			const int16_t osetPX = pX + pgxcb->offsetX;
			const int16_t osetPY = pY + pgxcb->offsetY;
#if 0
			printf("RENDER px: %d\npy: %d\nindex: %d\nox: %d\noy: %d\nwidth: %d\nheight: %d\n\n",
					pX, pY, line->xcbButtonIndex,
					osetPX, osetPY,
					pgxcb->window->width,
					pgxcb->window->height);
#endif
			protocol_Links_render(&pgxcb->links,
					line->xcbButtonIndex,
					osetPX, osetPY,
					pgxcb->window->width,
					pgxcb->window->height);

			pY += 10;
		}	break;
		default:
			break;

		}
	} 
	return pY;
}

void
p_gopher_Xcb_itemsInit(struct protocol_Xcb *pgxcb,
		struct p_gopher_Parser *parser)
{
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		struct p_gopher_Parser_Line *line = &parser->array[i];
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
			line->xcbButtonIndex = protocol_Links_new(&pgxcb->links,
					line->info, line->selector,
					pgxcb->window->id);
			break;
		default:
			break;
		}
	}
}

void
p_gopher_Xcb_scroll(struct protocol_Xcb *pgxcb,
		const struct p_gopher_Parser *parser)
{
	static const uint32_t yChange = 20;

	const uint32_t pX = 10;
	uint32_t pY = 10;
	for (uint32_t i = 0; i < parser->length; ++i, pY += yChange)
	{
		const struct p_gopher_Parser_Line *line = &parser->array[i];

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
		{
			const int16_t osetPX = pX + pgxcb->offsetX;
			const int16_t osetPY = pY + pgxcb->offsetY;
#if 0
			printf("SCROLL: px: %d\npy: %d\nindex: %d\nox: %d\noy: %d\nwidth: %d\nheight: %d\n\n",
					pX, pY, line->xcbButtonIndex,
					osetPX, osetPY,
					pgxcb->window->width,
					pgxcb->window->height);
#endif
			protocol_Links_render(&pgxcb->links,
					line->xcbButtonIndex,
					osetPX, osetPY,
					pgxcb->window->width,
					pgxcb->window->height);
			pY += 10;
		}	break;
		default:
			break;

		}
	} 
}

