#include <stdio.h>

#include "util/memory.h"

#include "gemini/parser.h"
#include "gemini/client.h"
#include "gemini/header.h"

#include "gopher/parser.h"
#include "gopher/client.h"

#include "ui/xcb/context.h"
#include "ui/xcb/key.h"
#include "ui/xcb/window.h"
#include "ui/xcb/event.h"
#include "ui/xcb/text.h"
#include "ui/xcb/pixmap.h"

#define ENABLE_XCB 0

uint32_t resizeMainContent(struct ui_xcb_Pixmap *mainArea,
		struct ui_xcb_Text *text,
		const struct gemini_Parser *parser,
		const uint32_t width,
		const uint32_t height,
		const uint32_t yOffset);

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

#if 0
	struct gemini_Parser parser = { 0 };
	gemini_Parser_init(&parser);
	//gemini_Parser_parseFp(&parser, tmpf);
	gemini_Parser_parse(&parser, "example/out.gmi");
	//gemini_Parser_parse(&parser, "example/test.gmi");
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

	// Gopher
	
#if 0
	FILE *tmpfp = fopen("example/out.gopher", "w+");
	
	struct gopher_Client goClient = { 0 };
	gopher_Client_init(&goClient, "gopher://gopher.quux.org/1/");
	gopher_Client_printInfo(&goClient);
	enum gopher_Client_ConnectError connErr = gopher_Client_request(&goClient, tmpfp);
	if (connErr != GOPHER_CLIENT_CONNECTERROR_NONE)
	{
		fprintf(stderr, "Gopher client ERROR CODE: %d\n",
				connErr);
	}
	gopher_Client_deinit(&goClient);

	// Print it all out
	rewind(tmpfp);
	char tmpfpBuffer[1024] = { 0 };
	while (fgets(tmpfpBuffer, sizeof(tmpfpBuffer), tmpfp) != NULL)
	{
		printf("tmpfp: %s", tmpfpBuffer);
	}

	fclose(tmpfp);
#endif

	struct gopher_Parser goParser = { 0 };
	gopher_Parser_init(&goParser);
	gopher_Parser_parse(&goParser, "example/out.gopher");
	//gopher_Parser_render(&goParser);
	//gopher_Parser_deinit(&goParser);

	// XCB TEMPS
	
#if 0
	struct ui_xcb_Context context = { 0 };
	ui_xcb_Context_init(&context);

	struct ui_xcb_Window window = { 0 };
	ui_xcb_Window_init(&window, &context, "gemcx");
	ui_xcb_Window_map(&window, true);

	struct ui_xcb_Event event = { 0 };
	ui_xcb_Event_init(&event, context.connection);

	const char *fontConfigs[5] = {
		"noto sans normal 12",
		"noto sans bold 24",
		"noto sans bold 18",
		"noto sans bold 14",
		"liberation mono 12"
	};
	struct ui_xcb_Text text[5] = { 0 };
	for (uint32_t i = 0; i < 5; ++i)
	{
		ui_xcb_Text_init(&text[i], &context, fontConfigs[i]);
	}

	struct ui_xcb_Pixmap doubleBuffer = { 0 };
	ui_xcb_Pixmap_init(&doubleBuffer, &context, window.id, 1920, 1080, 0x000000);

	struct ui_xcb_Pixmap mainArea = { 0 };
	ui_xcb_Pixmap_init(&mainArea, &context, doubleBuffer.pixmap, 1920, 10080, 0x222222);
	int32_t mainAreaYoffset = 0;
	uint32_t mainAreaXMax = 0;

	uint32_t windowWidth = 0;
	uint32_t windowHeight = 0;
	(void) windowWidth;
	(void) windowHeight;

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
			static uint32_t prevWidth = 0;
			static uint32_t mainAreaXMaxDuringRZ = 0;
			xcb_configure_notify_event_t *cnEvent =
				(xcb_configure_notify_event_t *) event.generic_event;

			if (prevWidth != cnEvent->width)
			{
				mainAreaXMax = resizeMainContent(&mainArea,
						text, &parser,
						cnEvent->width - 75,
						cnEvent->height,
						mainAreaYoffset);
				ui_xcb_Pixmap_render(&mainArea, 0, -mainAreaYoffset);
				mainAreaXMaxDuringRZ = mainAreaXMax;
			}
			else
			{
				mainAreaXMax = mainAreaXMaxDuringRZ;
			}

			if (cnEvent->height > mainAreaXMax)
			{
				mainAreaXMax = 0;
			}
			else
			{
				mainAreaXMax -= cnEvent->height;
			}
			prevWidth = cnEvent->width;

			windowWidth = cnEvent->width;
			windowHeight = cnEvent->height;
		} 	break;
		case XCB_EXPOSE:
		{
			xcb_expose_event_t *expose = (xcb_expose_event_t *) event.generic_event;
			(void) expose;
			ui_xcb_Pixmap_render(&doubleBuffer, 0, 0);
		}	break;
		case XCB_BUTTON_PRESS:
		{
			static const uint32_t offsetSpeed = 40;
			static const uint32_t maxOffset = 100;
			xcb_button_press_event_t *bp = (xcb_button_press_event_t *) event.generic_event;
			switch (bp->detail)
			{
			case 4:
				mainAreaYoffset -= offsetSpeed;
				if (mainAreaYoffset <= 0)
				{
					mainAreaYoffset = 0;
				}
				break;
			case 5:
				mainAreaYoffset += offsetSpeed;
				if (mainAreaYoffset >= (mainAreaXMax + maxOffset))
				{
					mainAreaYoffset = mainAreaXMax + maxOffset;
				}
				break;
			default:
				break;
			}

			switch (bp->detail)
			{
			case 4:
			case 5:
				ui_xcb_Pixmap_render(&mainArea, 0, -mainAreaYoffset);
				ui_xcb_Pixmap_render(&doubleBuffer, 0, 0);
				break;
			default:
				break;
			}
		}	break;
		case XCB_KEY_PRESS:
		{
		}	break;
		default:
			break;
		}
	}

	ui_xcb_Pixmap_deinit(&doubleBuffer);
	ui_xcb_Pixmap_deinit(&mainArea);
	for (uint32_t i = 0; i < 5; ++i)
	{
		ui_xcb_Text_deinit(&text[i]);
	}
	ui_xcb_Event_deinit(&event);
	ui_xcb_Window_deinit(&window);
	ui_xcb_Context_deinit(&context);

	ui_xcb_Text_GDEINIT();
#endif

	util_memory_freeAll();
	return 0;
}

#if 0
uint32_t
resizeMainContent(struct ui_xcb_Pixmap *mainArea,
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
#endif

