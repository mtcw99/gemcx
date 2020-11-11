#include "gopher/xcb.h"

uint32_t
gopher_Xcb_render(const struct gopher_Parser *parser,
		struct ui_xcb_Pixmap *pixmap,
		struct ui_xcb_Text *text)
{
	static const uint32_t yChange = 20;
	ui_xcb_Pixmap_clear(pixmap);

	const uint32_t pX = 10;
	uint32_t pY = 10;
	for (uint32_t i = 0; i < parser->length; ++i, pY += yChange)
	{
		const struct gopher_Parser_Line *line = &parser->array[i];

		switch (line->type)
		{
		case 'i':	// Informational Text
			ui_xcb_Text_render(text, pixmap->pixmap, line->info,
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
			ui_xcb_Text_render(text, pixmap->pixmap, line->info,
					pX, pY, 0xFFFFFF, 1.0);
			break;
		default:
			break;

		}
	}

	return pY;
}

