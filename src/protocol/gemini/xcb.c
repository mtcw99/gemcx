#include "protocol/gemini/xcb.h"

uint32_t
p_gemini_Xcb_render(const struct p_gemini_Parser *parser,
		struct ui_xcb_Pixmap *pixmap,
		struct ui_xcb_Text *text,
		const uint32_t width,
		const uint32_t height,
		const uint32_t yOffset)
{
	static const uint32_t yChange = 20;

	ui_xcb_Pixmap_clear(pixmap);

	uint32_t pY = 10;
	for (uint32_t i = 0; i < parser->length; ++i, pY += yChange)
	{
#if 0
		if (pY < yOffset)
		{
			continue;
		}
		else if (pY > (yOffset + height + 50))
		{
			break;
		}
#endif

		const struct p_gemini_Parser_Line *line = &parser->array[i];

		switch (line->type)
		{
		case P_GEMINI_PARSER_TYPE_HEAD:
			ui_xcb_Text_render(&text[line->content.head.level],
					pixmap->pixmap,
					line->content.head.text,
					10, pY, 0xFFFFFF, 1);
			switch (line->content.head.level)
			{
			case 1:	pY += 35; break;
			case 2:	pY += 25; break;
			case 3:	pY += 15; break;
			default:	break;
			}
			break;
		case P_GEMINI_PARSER_TYPE_LINK:
			ui_xcb_Text_render(&text[0],
					pixmap->pixmap,
					"=>",
					10, pY, 0xFFFFFF, 1);

			ui_xcb_Text_render(&text[0],
					pixmap->pixmap,
					line->content.link.text,
					25, pY, 0xFFFFFF, 1);
			break;
		case P_GEMINI_PARSER_TYPE_LIST:
			ui_xcb_Text_render(&text[0],
					pixmap->pixmap,
					"*",
					10, pY, 0xFFFFFF, 1);

			ui_xcb_Text_render(&text[0],
					pixmap->pixmap,
					line->content.link.text,
					20, pY, 0xFFFFFF, 1);
			break;
		case P_GEMINI_PARSER_TYPE_TEXT:
		case P_GEMINI_PARSER_TYPE_BLOCKQUOTES:
		{
			const double addY = ui_xcb_Text_renderWrapped(&text[0],
					pixmap->pixmap,
					line->content.text,
					10, pY,
					(line->type == P_GEMINI_PARSER_TYPE_TEXT) ? 0xFFFFFF : 0xAAAAAA,
					1, width, yChange);

			pY += addY - yChange;
		}	break;
		case P_GEMINI_PARSER_TYPE_PREFORMATTED:
			ui_xcb_Text_render(&text[4],
					pixmap->pixmap,
					line->content.text,
					10, pY, 0xFFFFFF, 1);
			break;
		default:
			break;
		}
	}

	return pY;
}

