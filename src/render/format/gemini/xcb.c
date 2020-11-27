#include "render/format/gemini/xcb.h"

uint32_t
render_format_gemini_Xcb_render(struct render_Xcb *pgxcb,
		const struct parser_format_Gemini *parser,
		const bool scroll)
{
	static const uint32_t yChange = 20;

	if (!scroll)
	{
		ui_xcb_Pixmap_clear(&pgxcb->pixmap);
	}

	const uint32_t pX = pgxcb->paddingLeft;
	uint32_t pY = 10;
	for (uint32_t i = 0; i < parser->length; ++i, pY += yChange)
	{
		const struct parser_format_Gemini_Line *line = &parser->array[i];

		switch (line->type)
		{
		case PARSER_FORMAT_GEMINI_TYPE_HEAD:
			if (!scroll)
			{
				ui_xcb_Text_render(&pgxcb->font[line->content.head.level],
						pgxcb->pixmap.pixmap,
						line->content.head.text,
						pX, pY, 0xFFFFFF, 1);
			}
			switch (line->content.head.level)
			{
			case 1:	pY += 35; break;
			case 2:	pY += 25; break;
			case 3:	pY += 15; break;
			default:	break;
			}
			break;
		case PARSER_FORMAT_GEMINI_TYPE_LINK:
		{
			// TODO
			const int16_t osetPY = pY + pgxcb->offsetY;

			if (!scroll)
			{
				ui_xcb_Text_render(&pgxcb->font[0],
						pgxcb->pixmap.pixmap,
						"►",
						pX, pY, 0xFFFFFF, 1);
			}

			render_Links_render(&pgxcb->links,
					line->content.link.xcbButtonIndex,
					20, osetPY,
					pgxcb->subwindow->rect.width,
					pgxcb->subwindow->rect.height);

		}	break;
		case PARSER_FORMAT_GEMINI_TYPE_LIST:
			if (!scroll)
			{
				ui_xcb_Text_render(&pgxcb->font[0],
						pgxcb->pixmap.pixmap,
						"*",
						pX, pY, 0xFFFFFF, 1);

				ui_xcb_Text_render(&pgxcb->font[0],
						pgxcb->pixmap.pixmap,
						line->content.link.text,
						20, pY, 0xFFFFFF, 1);
			}
			break;
		case PARSER_FORMAT_GEMINI_TYPE_TEXT:
		case PARSER_FORMAT_GEMINI_TYPE_BLOCKQUOTES:
		{
			double addY = 0;

			if (scroll)
			{
				addY = ui_xcb_Text_fakeRenderWrapped(&pgxcb->font[0],
						line->content.text,
						pY,
						pgxcb->subwindow->rect.width -
							pgxcb->paddingLeft -
							pgxcb->paddingRight,
						yChange).height;
			}
			else
			{
				addY = ui_xcb_Text_renderWrapped(&pgxcb->font[0],
						pgxcb->pixmap.pixmap,
						line->content.text,
						pX,
						pY,
						(line->type == PARSER_FORMAT_GEMINI_TYPE_TEXT) ? 0xFFFFFF : 0xAAAAAA,
						1,
						true,
						pgxcb->subwindow->rect.width -
							pgxcb->paddingLeft -
							pgxcb->paddingRight,
						yChange,
						true).height;
			}

			pY += addY - yChange;
		}	break;
		case PARSER_FORMAT_GEMINI_TYPE_PREFORMATTED:
			if (!scroll)
			{
				ui_xcb_Text_render(&pgxcb->font[4],
						pgxcb->pixmap.pixmap,
						line->content.text,
						pX, pY, 0xFFFFFF, 1);
			}
			break;
		default:
			break;
		}
	}

	return pY;
}

void
render_format_gemini_Xcb_itemsInit(struct render_Xcb *pgxcb,
		struct parser_format_Gemini *parser)
{
	for (uint32_t i = 0; i < parser->length; ++i)
	{
		struct parser_format_Gemini_Line *line = &parser->array[i];
		switch (line->type)
		{
		case PARSER_FORMAT_GEMINI_TYPE_LINK:
			line->content.link.xcbButtonIndex = render_Links_new(
					&pgxcb->links,
					line->content.link.text,
					line->content.link.link,
					pgxcb->subwindow->id);
			break;
		default:
			break;
		}
	}
}

