#include <stdio.h>

#include "util/memory.h"
#include "gemini/parser.h"
#include "gemini/client.h"
#include "gemini/header.h"
#include "ui/xcb/context.h"
#include "ui/xcb/key.h"
#include "ui/xcb/window.h"
#include "ui/xcb/event.h"
#include "ui/xcb/text.h"
#include "ui/xcb/pixmap.h"

void resizeMainContent(struct ui_xcb_Pixmap *mainArea,
		struct ui_xcb_Text *text,
		const struct gemini_Parser *parser,
		const uint32_t width);

int
main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	util_memory_enableDebug();

#if 0
	if (argc < 2)
	{
		return 1;
	}
	gemini_Client_GINIT();

	FILE *tmpf = tmpfile();

	struct gemini_Client client = { 0 };
	gemini_Client_init(&client, argv[1]);
	gemini_Client_printInfo(&client);
	enum gemini_Client_ConnectError connErr = gemini_Client_request(&client, tmpf);
	if (connErr != GEMINI_CLIENT_CONNECTERROR_NONE)
	{
		fprintf(stderr, "ERROR %d has occured: %s\n",
				connErr, gemini_Client_getErrorStr(connErr));
		return 1;
	}

	struct gemini_Header header = { 0 };
	gemini_Header_get(&header, tmpf);
	gemini_Header_print(&header);
#endif

#if 1
	struct gemini_Parser parser = { 0 };
	gemini_Parser_init(&parser);
	//gemini_Parser_parseFp(&parser, tmpf);
	//gemini_Parser_parse(&parser, "example/out.gmi");
	gemini_Parser_parse(&parser, "example/test.gmi");
	//gemini_Parser_print(&parser);
	//gemini_Parser_render(&parser);
#endif

#if 0
	FILE *givenFile = fopen("example/out.gmi", "w");
	rewind(tmpf);
	if (fgets(header, 1027, tmpf) != NULL)
	{
		printf("header: %s\n", header);
	}

	while (fgets(header, 1027, tmpf) != NULL)
	{
		fprintf(givenFile, "%s", header);
	}

	fclose(givenFile);
#endif

#if 0
	gemini_Header_print(&header);
	if (gemini_Header_isGemini(&header))
	{
		printf("Gemini header\n");
	}

	fclose(tmpf);
#endif

	// XCB TEMPS
	
	struct ui_xcb_Context context = { 0 };
	ui_xcb_Context_init(&context);

	struct ui_xcb_Window window = { 0 };
	ui_xcb_Window_init(&window, &context, "gemcx");
	ui_xcb_Window_map(&window, true);

	struct ui_xcb_Event event = { 0 };
	ui_xcb_Event_init(&event, context.connection);

	struct ui_xcb_Text text = { 0 };
	ui_xcb_Text_init(&text, &context, "Noto Sans Normal 12");

	struct ui_xcb_Pixmap doubleBuffer = { 0 };
	ui_xcb_Pixmap_init(&doubleBuffer, &context, window.id, 1920, 1080, 0x000000);

	// TEMP: parser to render testing
	struct ui_xcb_Pixmap mainArea = { 0 };
	ui_xcb_Pixmap_init(&mainArea, &context, doubleBuffer.pixmap, 1920, 1080, 0x222222);

	xcb_flush(context.connection);
	while (ui_xcb_Event_waitForEvent(&event))
	{
		switch (event.response_type)
		{
		case XCB_CLIENT_MESSAGE:
		{
			xcb_client_message_event_t *clientMsgEvent = 
				(xcb_client_message_event_t *) event.generic_event;
			if (clientMsgEvent->data.data32[0] ==
					context.exit_reply->atom)
			{
				if (clientMsgEvent->window == window.id)
				{
					ui_xcb_Event_close(&event);
				}
				else
				{
				}
			}
		}	break;
		case XCB_CONFIGURE_NOTIFY:
		{
			xcb_configure_notify_event_t *cnEvent =
				(xcb_configure_notify_event_t *) event.generic_event;
			resizeMainContent(&mainArea, &text, &parser, cnEvent->width - 100);
			ui_xcb_Pixmap_render(&mainArea, 0, 0);
		} 	break;
		case XCB_EXPOSE:
		{
			xcb_expose_event_t *expose = (xcb_expose_event_t *) event.generic_event;
			(void) expose;
			ui_xcb_Pixmap_render(&doubleBuffer, 0, 0);
		}	break;
		case XCB_BUTTON_PRESS:
		{
		}	break;
		default:
			break;
		}
	}

	ui_xcb_Pixmap_deinit(&doubleBuffer);
	ui_xcb_Pixmap_deinit(&mainArea);
	ui_xcb_Text_deinit(&text);
	ui_xcb_Event_deinit(&event);
	ui_xcb_Window_deinit(&window);
	ui_xcb_Context_deinit(&context);

	ui_xcb_Text_GDEINIT();

	util_memory_freeAll();
	return 0;
}

void
resizeMainContent(struct ui_xcb_Pixmap *mainArea,
		struct ui_xcb_Text *text,
		const struct gemini_Parser *parser,
		const uint32_t width)
{
	ui_xcb_Pixmap_clear(mainArea);
	for (uint32_t i = 0, pY = 10; i < parser->length; ++i, pY += 15)
	{
		const struct gemini_Parser_Line *line = &parser->array[i];

		switch (line->type)
		{
		case GEMINI_PARSER_TYPE_HEAD:
			ui_xcb_Text_render(text,
					mainArea->pixmap,
					"#",
					10, pY, 0xFFFFFF, 1);

			ui_xcb_Text_render(text,
					mainArea->pixmap,
					line->content.head.text,
					20, pY, 0xFFFFFF, 1);
			break;
		case GEMINI_PARSER_TYPE_LINK:
			ui_xcb_Text_render(text,
					mainArea->pixmap,
					"=>",
					10, pY, 0xFFFFFF, 1);

			ui_xcb_Text_render(text,
					mainArea->pixmap,
					line->content.link.text,
					25, pY, 0xFFFFFF, 1);
			break;
		case GEMINI_PARSER_TYPE_LIST:
			ui_xcb_Text_render(text,
					mainArea->pixmap,
					"*",
					10, pY, 0xFFFFFF, 1);

			ui_xcb_Text_render(text,
					mainArea->pixmap,
					line->content.link.text,
					20, pY, 0xFFFFFF, 1);
			break;
			break;
		case GEMINI_PARSER_TYPE_TEXT:
		case GEMINI_PARSER_TYPE_BLOCKQUOTES:
		{
			const double addY = ui_xcb_Text_renderWrapped(text,
					mainArea->pixmap,
					line->content.text,
					10, pY, 0xFFFFFF, 1, width);

			pY += addY - 15;
		}	break;
		case GEMINI_PARSER_TYPE_PREFORMATTED:
			ui_xcb_Text_render(text,
					mainArea->pixmap,
					line->content.text,
					10, pY, 0xFFFFFF, 1);
			break;
		default:
			break;
		}
	}
}

