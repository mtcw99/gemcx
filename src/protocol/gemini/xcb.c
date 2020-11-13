#include "protocol/gemini/xcb.h"

uint32_t
p_gemini_Xcb_render(struct protocol_Xcb *pgxcb,
		const struct p_gemini_Parser *parser,
		const uint32_t width,
		const uint32_t height)
{
	static const uint32_t yChange = 20;

	ui_xcb_Pixmap_clear(&pgxcb->pixmap);

	uint32_t pY = 10;
	for (uint32_t i = 0; i < parser->length; ++i, pY += yChange)
	{
		const struct p_gemini_Parser_Line *line = &parser->array[i];

		switch (line->type)
		{
		case P_GEMINI_PARSER_TYPE_HEAD:
			ui_xcb_Text_render(&pgxcb->font[line->content.head.level],
					pgxcb->pixmap.pixmap,
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
			ui_xcb_Text_render(&pgxcb->font[0],
					pgxcb->pixmap.pixmap,
					"=>",
					10, pY, 0xFFFFFF, 1);

			ui_xcb_Text_render(&pgxcb->font[0],
					pgxcb->pixmap.pixmap,
					line->content.link.text,
					25, pY, 0xFFFFFF, 1);
			break;
		case P_GEMINI_PARSER_TYPE_LIST:
			ui_xcb_Text_render(&pgxcb->font[0],
					pgxcb->pixmap.pixmap,
					"*",
					10, pY, 0xFFFFFF, 1);

			ui_xcb_Text_render(&pgxcb->font[0],
					pgxcb->pixmap.pixmap,
					line->content.link.text,
					20, pY, 0xFFFFFF, 1);
			break;
		case P_GEMINI_PARSER_TYPE_TEXT:
		case P_GEMINI_PARSER_TYPE_BLOCKQUOTES:
		{
			const double addY = ui_xcb_Text_renderWrapped(&pgxcb->font[0],
					pgxcb->pixmap.pixmap,
					line->content.text,
					10, pY,
					(line->type == P_GEMINI_PARSER_TYPE_TEXT) ? 0xFFFFFF : 0xAAAAAA,
					1, width, yChange);

			pY += addY - yChange;
		}	break;
		case P_GEMINI_PARSER_TYPE_PREFORMATTED:
			ui_xcb_Text_render(&pgxcb->font[4],
					pgxcb->pixmap.pixmap,
					line->content.text,
					10, pY, 0xFFFFFF, 1);
			break;
		default:
			break;
		}
	}

	return pY;
}

void
p_gemini_Xcb_itemsInit(struct protocol_Xcb *pgxcb,
		const struct p_gemini_Parser *parser)
{

}

