#include <stdio.h>

#include "util/memory.h"

#include "protocol/client.h"
#include "protocol/parser.h"
#include "protocol/xcb.h"

#include "protocol/gemini/header.h"

#include "ui/xcb/context.h"
#include "ui/xcb/key.h"
#include "ui/xcb/window.h"
#include "ui/xcb/event.h"
#include "ui/xcb/text.h"
#include "ui/xcb/pixmap.h"

#define ENABLE_XCB 0

int
main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	util_memory_enableDebug();

	struct protocol_Parser parser = { 0 };
#if 1
	protocol_Parser_init(&parser, PROTOCOL_TYPE_GOPHER);
	protocol_Parser_parse(&parser, "example/out.gopher");
#else
	protocol_Parser_init(&parser, PROTOCOL_TYPE_GEMINI);
	protocol_Parser_parse(&parser, "example/out.gmi");
#endif

	// XCB TEMPS
	
	struct ui_xcb_Context context = { 0 };
	ui_xcb_Context_init(&context);

	struct ui_xcb_Window window = { 0 };
	ui_xcb_Window_init(&window, &context, "gemcx");
	ui_xcb_Window_map(&window, true);

	struct ui_xcb_Event event = { 0 };
	ui_xcb_Event_init(&event, context.connection);

	const char *fontConfigs[UI_XCB_TEXTTYPE__TOTAL] = {
		[UI_XCB_TEXTTYPE_TEXT] = "noto sans normal 12",
		[UI_XCB_TEXTTYPE_H1] = "noto sans bold 24",
		[UI_XCB_TEXTTYPE_H2] = "noto sans bold 18",
		[UI_XCB_TEXTTYPE_H3] = "noto sans bold 14",
		[UI_XCB_TEXTTYPE_PRE] = "liberation mono 12"
	};
	struct ui_xcb_Text text[UI_XCB_TEXTTYPE__TOTAL] = { 0 };
	for (uint32_t i = 0; i < 5; ++i)
	{
		ui_xcb_Text_init(&text[i], &context, fontConfigs[i]);
	}

	struct ui_xcb_Pixmap doubleBuffer = { 0 };
	ui_xcb_Pixmap_init(&doubleBuffer, &context, window.id, 1920, 1080, 0x000000);

	struct ui_xcb_Pixmap mainArea = { 0 };
	ui_xcb_Pixmap_init(&mainArea, &context, doubleBuffer.pixmap, 1920, 1080, 0x222222);
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
				mainAreaXMax = protocol_Xcb_render(&parser,
						&mainArea,
						(parser.type == PROTOCOL_TYPE_GEMINI) ?
						 	text :
							&text[UI_XCB_TEXTTYPE_PRE],
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

	// xcb deinit
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

	// protocols deinit
	protocol_Parser_deinit(&parser);

	util_memory_freeAll();
	return 0;
}



#if 0
	if (argc < 2)
	{
		return 1;
	}
	p_gemini_Client_GINIT();

	FILE *tmpf = tmpfile();

	struct p_gemini_Client client = { 0 };
	p_gemini_Client_init(&client, argv[1]);
	p_gemini_Client_printInfo(&client);
	enum p_gemini_Client_ConnectError connErr = p_gemini_Client_request(&client, tmpf);
	if (connErr != GEMINI_CLIENT_CONNECTERROR_NONE)
	{
		fprintf(stderr, "ERROR %d has occured: %s\n",
				connErr, p_gemini_Client_getErrorStr(connErr));
		return 1;
	}

	struct p_gemini_Header header = { 0 };
	p_gemini_Header_get(&header, tmpf);
	p_gemini_Header_print(&header);
	struct p_gemini_Parser parser = { 0 };
	p_gemini_Parser_init(&parser);
	//p_gemini_Parser_parseFp(&parser, tmpf);
	p_gemini_Parser_parse(&parser, "example/out.gmi");
	//p_gemini_Parser_parse(&parser, "example/test.gmi");
	//p_gemini_Parser_print(&parser);
	//p_gemini_Parser_render(&parser);

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

	p_gemini_Header_print(&header);
	if (p_gemini_Header_isGemini(&header))
	{
		printf("Gemini header\n");
	}

	fclose(tmpf);

	// Gopher
	
	FILE *tmpfp = fopen("example/out.gopher", "w+");
	
	struct p_gopher_Client goClient = { 0 };
	p_gopher_Client_init(&goClient, "gopher://gopher.quux.org/1/");
	p_gopher_Client_printInfo(&goClient);
	enum p_gopher_Client_ConnectError connErr = p_gopher_Client_request(&goClient, tmpfp);
	if (connErr != GOPHER_CLIENT_CONNECTERROR_NONE)
	{
		fprintf(stderr, "Gopher client ERROR CODE: %d\n",
				connErr);
	}
	p_gopher_Client_deinit(&goClient);

	// Print it all out
	rewind(tmpfp);
	char tmpfpBuffer[1024] = { 0 };
	while (fgets(tmpfpBuffer, sizeof(tmpfpBuffer), tmpfp) != NULL)
	{
		printf("tmpfp: %s", tmpfpBuffer);
	}

	fclose(tmpfp);

	struct p_gopher_Parser goParser = { 0 };
	p_gopher_Parser_init(&goParser);
	p_gopher_Parser_parse(&goParser, "example/out.gopher");
	//p_gopher_Parser_render(&goParser);
	//p_gopher_Parser_deinit(&goParser);

#endif

