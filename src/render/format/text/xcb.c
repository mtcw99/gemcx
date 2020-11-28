#include "render/format/text/xcb.h"

uint32_t
render_format_text_Xcb_render(struct render_Xcb *pgxcb,
		const struct parser_format_Text *fText,
		const bool scroll)
{
	static const uint32_t yChange = 20;
	if (!scroll)
	{
		ui_xcb_Pixmap_clear(&pgxcb->pixmap);
	}

	const uint32_t pX = 10;
	uint32_t pY = 10;
	for (uint32_t i = 0; i < fText->length; ++i, pY += yChange)
	{
		const struct parser_format_Text_Line *line = &fText->array[i];
		if (!scroll)
		{
			ui_xcb_Text_render(&pgxcb->font[4],
					pgxcb->pixmap.pixmap,
					line->str,
					pX, pY,
					0xFFFFFF, 1.0);
		}
	}
	return pY;
}

