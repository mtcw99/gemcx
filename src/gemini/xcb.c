#include "gemini/xcb.h"

uint32_t
gemini_Xcb_render(struct ui_xcb_Pixmap *mainArea,
		struct ui_xcb_Text *text,
		const struct gemini_Parser *parser,
		const uint32_t width,
		const uint32_t height,
		const uint32_t yOffset)
{
	static const uint32_t yChange = 20;

	ui_xcb_Pixmap_clear(mainArea);

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

		const struct gemini_Parser_Line *line = &parser->array[i];

		switch (line->type)
		{
		case GEMINI_PARSER_TYPE_HEAD:
			ui_xcb_Text_render(&text[line->content.head.level],
					mainArea->pixmap,
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
		case GEMINI_PARSER_TYPE_LINK:
			ui_xcb_Text_render(&text[0],
					mainArea->pixmap,
					"=>",
					10, pY, 0xFFFFFF, 1);

			ui_xcb_Text_render(&text[0],
					mainArea->pixmap,
					line->content.link.text,
					25, pY, 0xFFFFFF, 1);
			break;
		case GEMINI_PARSER_TYPE_LIST:
			ui_xcb_Text_render(&text[0],
					mainArea->pixmap,
					"*",
					10, pY, 0xFFFFFF, 1);

			ui_xcb_Text_render(&text[0],
					mainArea->pixmap,
					line->content.link.text,
					20, pY, 0xFFFFFF, 1);
			break;
		case GEMINI_PARSER_TYPE_TEXT:
		case GEMINI_PARSER_TYPE_BLOCKQUOTES:
		{
			const double addY = ui_xcb_Text_renderWrapped(&text[0],
					mainArea->pixmap,
					line->content.text,
					10, pY,
					(line->type == GEMINI_PARSER_TYPE_TEXT) ? 0xFFFFFF : 0xAAAAAA,
					1, width, yChange);

			pY += addY - yChange;
		}	break;
		case GEMINI_PARSER_TYPE_PREFORMATTED:
			ui_xcb_Text_render(&text[4],
					mainArea->pixmap,
					line->content.text,
					10, pY, 0xFFFFFF, 1);
			break;
		default:
			break;
		}
	}

	return pY;
}

