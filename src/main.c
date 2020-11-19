#include <stdio.h>

#include "util/memory.h"
#include "util/socket.h"
#include "util/stack.h"

#include "protocol/client.h"
#include "protocol/parser.h"
#include "protocol/xcb.h"
#include "protocol/historyStack.h"

#include "protocol/gemini/header.h"

#include "ui/xcb/context.h"
#include "ui/xcb/key.h"
#include "ui/xcb/window.h"
#include "ui/xcb/event.h"
#include "ui/xcb/text.h"
#include "ui/xcb/pixmap.h"
#include "ui/xcb/subwindow.h"
#include "ui/xcb/button.h"
#include "ui/xcb/cursor.h"
#include "ui/xcb/subwindow.h"
#include "ui/xcb/textInput.h"
#include "ui/xcb/clipboard.h"
#include "ui/xcb/menu.h"

#include "gemcx/xcb/controlBar.h"
#include "gemcx/xcb/connectUrl.h"

static char *
firstNonWhiteSpace(char *str)
{
	char *fStr = str;
	while (*fStr == ' ')
		++fStr;
	return fStr;
}

int
main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

#ifdef DEBUG
	util_memory_enableDebug();
	printf("NOTICE: RUNNING DEBUG BUILD\n");
#endif
	struct protocol_Client client = { 0 };
	struct protocol_Parser parser = { 0 };

	//const char *startUrl = "gopher://gopher.quux.org/1/";
	//const char *startUrl = "file://example/out.gopher";
	const char *startUrl = "gemini://gemini.circumlunar.space/";
	//const char *startUrl = "file://example/test.gmi";
	//const char *startUrl = "file://example/out.gmi";

	gemcx_xcb_ConnectUrl_connect(&client, &parser, startUrl);

	struct ui_xcb_Context context = { 0 };
	ui_xcb_Context_init(&context);

	struct ui_xcb_Clipboard clipboard = { 0 };
	ui_xcb_Clipboard_init(&clipboard, &context);

	struct ui_xcb_Window window = { 0 };
	ui_xcb_Window_init(&window, &context, "gemcx");
	ui_xcb_Window_minSize(&window, 400, 200);
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

	const int32_t contentSubWindowYDiff = 30;

	// Content sub-window
	struct ui_xcb_Subwindow contentSubWindow = { 0 };
	ui_xcb_Subwindow_init(&contentSubWindow, &context, window.id,
			0x222222, 0xEEEEEE, 0,
			(const xcb_rectangle_t) {
				.x = 0,
				.y = contentSubWindowYDiff,
				.width = 640,
				.height = 480
			},
			XCB_EVENT_MASK_EXPOSURE 	|
			XCB_EVENT_MASK_KEY_PRESS 	|
			XCB_EVENT_MASK_KEY_RELEASE 	|
			XCB_EVENT_MASK_BUTTON_PRESS 	|
			XCB_EVENT_MASK_BUTTON_RELEASE);
	ui_xcb_Subwindow_show(&contentSubWindow, true);

	char urlStr[1024] = { 0 };
	strcpy(urlStr, startUrl);

	struct gemcx_xcb_ControlBar controlBar = { 0 };
	gemcx_xcb_ControlBar_init(&controlBar, &context, window.id,
			contentSubWindowYDiff, urlStr, sizeof(urlStr), text);

	struct ui_xcb_Pixmap doubleBuffer = { 0 };
	ui_xcb_Pixmap_init(&doubleBuffer, &context, contentSubWindow.id,
			context.rootWidth, context.rootHeight, 0x000000);

	struct ui_xcb_Pixmap mainArea = { 0 };
	ui_xcb_Pixmap_init(&mainArea, &context, doubleBuffer.pixmap,
			context.rootWidth, 10080, 0x222222);
	int32_t mainAreaYoffset = 0;
	uint32_t mainAreaYMax = 0;

	struct ui_xcb_Key xkey = { 0 };
	ui_xcb_Key_init(&xkey, context.connection);

	struct protocol_Xcb pxcb = { 0 };
	protocol_Xcb_init(&pxcb, &context,
			text, mainArea.pixmap, &contentSubWindow,
			context.rootWidth, 10080, 0x222222);

	protocol_Xcb_itemsInit(&pxcb, &parser);
	protocol_Xcb_padding(&pxcb, 10, 10);

	struct protocol_HistoryStack historyStack = { 0 };
	protocol_HistoryStack_init(&historyStack);
	protocol_HistoryStack_push(&historyStack, urlStr);

	struct gemcx_xcb_ConnectUrl connectUrl = {
		.client = &client,
		.parser = &parser,
		.pxcb = &pxcb,
		.mainAreaYoffset = &mainAreaYoffset,
		.urlInput = &controlBar.urlInput,
		.mainAreaYMax = &mainAreaYMax,
		.mainArea = &mainArea,
		.doubleBuffer = &doubleBuffer,
		.historyStack = &historyStack
	};

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
		case XCB_SELECTION_NOTIFY:
		{
			// Paste operation
			ui_xcb_Clipboard_selectionNotify(&clipboard,
					(xcb_selection_notify_event_t *) event.generic_event);
			gemcx_xcb_ControlBar_selectionNotify(&controlBar, &clipboard);
		}	break;
		case XCB_SELECTION_REQUEST:
		{
			// Copy operation
			ui_xcb_Clipboard_selectionRequest(&clipboard,
					(xcb_selection_request_event_t *) event.generic_event);
		}	break;
		case XCB_PROPERTY_NOTIFY:
		{
		}	break;
		case XCB_SELECTION_CLEAR:
		{
		}	break;
		case XCB_MAP_NOTIFY:
		{
			xcb_map_notify_event_t *mapEvent =
				(xcb_map_notify_event_t *) event.generic_event;
			if (mapEvent->event == window.id)
			{
				if (!window.mapInit)
				{
					window.mapInit = true;
				}
			}
		}	break;
		case XCB_CONFIGURE_NOTIFY:
		{
			static uint32_t prevWidth = 0;
			static uint32_t mainAreaYMaxDuringRZ = 0;
			xcb_configure_notify_event_t *cnEvent =
				(xcb_configure_notify_event_t *) event.generic_event;

			// Ignore XCB_CONFIGURE_NOTIFY event if not window
			if (window.id != cnEvent->window)
			{
				break;
			}

			ui_xcb_Window_updateInfo(&window,
					cnEvent->width,
					cnEvent->height);

			// Update subwindow
			contentSubWindow.rect.width = cnEvent->width;
			contentSubWindow.rect.height = cnEvent->height - contentSubWindowYDiff - 2;
			ui_xcb_Subwindow_applyAttributes(&contentSubWindow);

			gemcx_xcb_ControlBar_configureNotify(&controlBar,
					cnEvent);

			if (prevWidth != cnEvent->width)
			{
				protocol_Xcb_offset(&pxcb, 0, -mainAreaYoffset);
				mainAreaYMax = protocol_Xcb_render(&pxcb,
						&parser);
				ui_xcb_Pixmap_render(&mainArea, 0, 0);
				ui_xcb_Pixmap_render(&doubleBuffer, 0, 0);
				mainAreaYMaxDuringRZ = mainAreaYMax;
			}
			else
			{
				mainAreaYMax = mainAreaYMaxDuringRZ;
			}

			if (cnEvent->height > mainAreaYMax)
			{
				// Window height more than content height
				mainAreaYMax = 0;
			}
			else
			{
				// Content height more than window height
				mainAreaYMax -= cnEvent->height - ((uint32_t) ((float) cnEvent->height * 0.5));
			}
			prevWidth = cnEvent->width;
		} 	break;
		case XCB_EXPOSE:
		{
			xcb_expose_event_t *expose = (xcb_expose_event_t *) event.generic_event;
			(void) expose;
			ui_xcb_Pixmap_render(&doubleBuffer, 0, 0);
			gemcx_xcb_ControlBar_expose(&controlBar, expose);
		}	break;
		case XCB_ENTER_NOTIFY:
		{
			xcb_enter_notify_event_t *enterEv =
				(xcb_enter_notify_event_t *) event.generic_event;
			if (gemcx_xcb_ControlBar_enterNotify(&controlBar, enterEv))
			{
			}
			else if (protocol_Xcb_hoverEnter(&pxcb, enterEv))
			{
			}
		}	break;
		case XCB_LEAVE_NOTIFY:
		{
			xcb_leave_notify_event_t *leaveEv =
				(xcb_leave_notify_event_t *) event.generic_event;
			if (gemcx_xcb_ControlBar_leaveNotify(&controlBar, leaveEv))
			{
			}
			else if (protocol_Xcb_hoverLeave(&pxcb, leaveEv))
			{
			}
		}	break;
		case XCB_BUTTON_PRESS:
		{
			static const uint32_t offsetSpeed = 40;
			xcb_button_press_event_t *bp =
				(xcb_button_press_event_t *) event.generic_event;

			switch (bp->detail)
			{
			case 1:	// Left-mouse click
			{
				bool connUrlRR = false;
				if (gemcx_xcb_ControlBar_buttonPress(&controlBar,
							bp,
							&event,
							&historyStack,
							&connUrlRR,
							urlStr))
				{
					if (connUrlRR)
					{
						gemcx_xcb_ConnectUrl_connectRender(
								&connectUrl,
								urlStr,
								false);
					}
					break;
				}

				for (uint32_t i = 0; i < pxcb.links.length; ++i)
				{
					if (protocol_Links_clicked(&pxcb.links, i, bp->event))
					{
						const struct protocol_Link *link = &pxcb.links.links[i];
						printf("%d: ref: %s\n", i, link->ref);
						// TODO: Connect to new URL
						if (util_socket_urlHasScheme(link->ref, strlen(link->ref)))
						{
							strcpy(urlStr, link->ref);
						}
						else
						{
							sprintf(urlStr, "%s://%s%s%s%s",
									client.host.scheme,
									client.host.hostname,
									(client.type == PROTOCOL_TYPE_GOPHER) ? "/1" : "",
									(link->ref[0] == '/') ? "" : "/",
									firstNonWhiteSpace(link->ref));
						}

						gemcx_xcb_ConnectUrl_connectRender(
								&connectUrl,
								urlStr,
								true);
						break;
					}
				}
			}	break;
			case 4:	// Scroll up
				mainAreaYoffset -= offsetSpeed;
				if (mainAreaYoffset <= 0)
				{
					mainAreaYoffset = 0;
				}
				break;
			case 5:	// Scroll down
				mainAreaYoffset += offsetSpeed;
				if (mainAreaYoffset >= mainAreaYMax)
				{
					mainAreaYoffset = mainAreaYMax;
				}
				break;
			default:
				break;
			}

			switch (bp->detail)
			{
			case 4:
			case 5:
				protocol_Xcb_offset(&pxcb, 0, -mainAreaYoffset);
				protocol_Xcb_scroll(&pxcb, &parser);
				if (bp->detail == 4)
				{
					// Rerender buttons: scroll up apparently
					// breaks rendering but not down
					protocol_Xcb_scroll(&pxcb, &parser);
				}

				ui_xcb_Pixmap_render(&mainArea, 0, 0);
				ui_xcb_Pixmap_render(&doubleBuffer, 0, 0);
				break;
			default:
				break;
			}
		}	break;
		case XCB_KEY_PRESS:
		{
			static const uint32_t offsetSpeed = 40;
			static const uint32_t pageOffsetSpeed = 400;

			xcb_key_press_event_t *prEv =
				(xcb_key_press_event_t *) event.generic_event;

			ui_xcb_Key_set(&xkey, prEv->detail, prEv->state);
			//printf("key: %s\n", xkey.buffer);

			// COPY: Ctrl-c
			if ((xkey.mask & XCB_KEY_BUT_MASK_CONTROL) &&
					(xkey.keysymNoMask == XKB_KEY_c))
			{
				clipboard.content = realloc(clipboard.content,
						sizeof(char) * 16);
				strcpy(clipboard.content, "TEST123");
				clipboard.contentLength = strlen("TEST123");

				ui_xcb_Clipboard_selectionSetOwner(&clipboard);
				break;
			}

			// PASTE: Ctrl-v
			if ((xkey.mask & XCB_KEY_BUT_MASK_CONTROL) &&
					(xkey.keysymNoMask == XKB_KEY_v))
			{
				ui_xcb_Clipboard_selectionCovert(&clipboard);
				break;
			}

			ui_xcb_TextInput_modify(&controlBar.urlInput, &xkey);

			switch (xkey.keysymNoMask)
			{
#if 0
			case XKB_KEY_Escape:
				ui_xcb_Event_close(&event);
				break;
#endif
			case XKB_KEY_Up:
			case XKB_KEY_Page_Up:
				mainAreaYoffset -= (xkey.keysymNoMask == XKB_KEY_Up) ? offsetSpeed : pageOffsetSpeed;
				if (mainAreaYoffset <= 0)
				{
					mainAreaYoffset = 0;
				}
				break;
			case XKB_KEY_Down:
			case XKB_KEY_Page_Down:
				mainAreaYoffset += (xkey.keysymNoMask == XKB_KEY_Down) ? offsetSpeed : pageOffsetSpeed;
				if (mainAreaYoffset >= mainAreaYMax)
				{
					mainAreaYoffset = mainAreaYMax;
				}
				break;
			case XKB_KEY_Return:
				if (controlBar.urlInput.active)
				{
					printf("urlStr: %s\n", urlStr);
					// Go to a new specified URL
					gemcx_xcb_ConnectUrl_connectRender(
							&connectUrl,
							urlStr,
							true);
				}
				break;
			default:
				break;
			}

			switch (xkey.keysymNoMask)
			{
			case XKB_KEY_Up:
			case XKB_KEY_Page_Up:
			case XKB_KEY_Down:
			case XKB_KEY_Page_Down:
				protocol_Xcb_offset(&pxcb, 0, -mainAreaYoffset);
				protocol_Xcb_scroll(&pxcb, &parser);
				if (xkey.keysymNoMask == XKB_KEY_Down ||
						xkey.keysymNoMask == XKB_KEY_Page_Down)
				{
					// Rerender buttons: scroll up apparently
					// breaks rendering but not down
					protocol_Xcb_scroll(&pxcb, &parser);
				}

				ui_xcb_Pixmap_render(&mainArea, 0, 0);
				ui_xcb_Pixmap_render(&doubleBuffer, 0, 0);
			default:
				break;
			}

		}	break;
		case XCB_KEY_RELEASE:
		{
			xcb_key_release_event_t *reEv =
				(xcb_key_release_event_t *) event.generic_event;

			ui_xcb_Key_set(&xkey, reEv->detail, reEv->state);
		}	break;
		default:
			break;
		}
	}

	protocol_HistoryStack_deinit(&historyStack);

	protocol_Xcb_deinit(&pxcb);
	gemcx_xcb_ControlBar_deinit(&controlBar);

	// xcb deinit
	ui_xcb_Key_deinit(&xkey);
	ui_xcb_Pixmap_deinit(&doubleBuffer);
	ui_xcb_Pixmap_deinit(&mainArea);
	for (uint32_t i = 0; i < 5; ++i)
	{
		ui_xcb_Text_deinit(&text[i]);
	}
	ui_xcb_Subwindow_deinit(&contentSubWindow);
	ui_xcb_Event_deinit(&event);
	ui_xcb_Window_deinit(&window);
	ui_xcb_Clipboard_deinit(&clipboard);
	ui_xcb_Context_deinit(&context);

	ui_xcb_Text_GDEINIT();

	// protocols deinit
	protocol_Parser_deinit(&parser);

	util_memory_freeAll();
	return 0;
}

