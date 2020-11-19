#include "gemcx/xcb/eventResponse.h"

#include "gemcx/xcb/globals.h"

static char *
firstNonWhiteSpace(char *str)
{
	char *fStr = str;
	while (*fStr == ' ')
		++fStr;
	return fStr;
}

// Events

static void
gemcx_xcb_EventResponse__clientMessage(const xcb_generic_event_t *genericEvent)
{
	xcb_client_message_event_t *clientMsgEvent = 
		(xcb_client_message_event_t *) genericEvent;

	if (clientMsgEvent->data.data32[0] ==
			globals.context.exit_reply->atom)
	{
		if (clientMsgEvent->window == globals.window.id)
		{
			ui_xcb_Event_close(&globals.event);
		}
	}
}

static void
gemcx_xcb_EventResponse__selectionNotify(const xcb_generic_event_t *genericEvent)
{
	// Paste operation
	ui_xcb_Clipboard_selectionNotify(&globals.clipboard,
			(xcb_selection_notify_event_t *) genericEvent);
	gemcx_xcb_ControlBar_selectionNotify(&globals.controlBar, &globals.clipboard);
}

static void
gemcx_xcb_EventResponse__selectionRequest(const xcb_generic_event_t *genericEvent)
{
	// Copy operation
	ui_xcb_Clipboard_selectionRequest(&globals.clipboard,
			(xcb_selection_request_event_t *) genericEvent);
}

static void
gemcx_xcb_EventResponse__mapNotify(const xcb_generic_event_t *genericEvent)
{
	xcb_map_notify_event_t *mapEvent = (xcb_map_notify_event_t *) genericEvent;
	if (mapEvent->event == globals.window.id)
	{
		if (!globals.window.mapInit)
		{
			globals.window.mapInit = true;
		}
	}
}

static void
gemcx_xcb_EventResponse__configureNotify(const xcb_generic_event_t *genericEvent)
{
	xcb_configure_notify_event_t *cnEvent = (xcb_configure_notify_event_t *) genericEvent;

	static uint32_t prevWidth = 0;
	static uint32_t mainAreaYMaxDuringRZ = 0;

	// Ignore XCB_CONFIGURE_NOTIFY event if not window
	if (globals.window.id != cnEvent->window)
	{
		return;
	}

	ui_xcb_Window_updateInfo(&globals.window, cnEvent->width, cnEvent->height);

	// Update subwindow
	globals.contentSubWindow.rect.width = cnEvent->width;
	globals.contentSubWindow.rect.height = cnEvent->height - globals.contentSubWindowYDiff - 2;
	ui_xcb_Subwindow_applyAttributes(&globals.contentSubWindow);

	gemcx_xcb_ControlBar_configureNotify(&globals.controlBar, cnEvent);

	if (prevWidth != cnEvent->width)
	{
		protocol_Xcb_offset(&globals.pxcb, 0, -globals.mainAreaYoffset);
		globals.mainAreaYMax = protocol_Xcb_render(&globals.pxcb,
				&globals.parser);
		ui_xcb_Pixmap_render(&globals.mainArea, 0, 0);
		ui_xcb_Pixmap_render(&globals.doubleBuffer, 0, 0);
		mainAreaYMaxDuringRZ = globals.mainAreaYMax;
	}
	else
	{
		globals.mainAreaYMax = mainAreaYMaxDuringRZ;
	}

	if (cnEvent->height > globals.mainAreaYMax)
	{
		// Window height more than content height
		globals.mainAreaYMax = 0;
	}
	else
	{
		// Content height more than window height
		globals.mainAreaYMax -= cnEvent->height - ((uint32_t) ((float) cnEvent->height * 0.5));
	}
	prevWidth = cnEvent->width;
}

static void
gemcx_xcb_EventResponse__expose(const xcb_generic_event_t *genericEvent)
{
	xcb_expose_event_t *expose = (xcb_expose_event_t *) genericEvent;
	ui_xcb_Pixmap_render(&globals.doubleBuffer, 0, 0);
	gemcx_xcb_ControlBar_expose(&globals.controlBar, expose);
}

static void
gemcx_xcb_EventResponse__enterNotify(const xcb_generic_event_t *genericEvent)
{
	xcb_enter_notify_event_t *enterEv = (xcb_enter_notify_event_t *) genericEvent;
	if (gemcx_xcb_ControlBar_enterNotify(&globals.controlBar, enterEv))
	{
	}
	else if (protocol_Xcb_hoverEnter(&globals.pxcb, enterEv))
	{
	}
}

static void
gemcx_xcb_EventResponse__leaveNotify(const xcb_generic_event_t *genericEvent)
{
	xcb_leave_notify_event_t *leaveEv = (xcb_leave_notify_event_t *) genericEvent;
	if (gemcx_xcb_ControlBar_leaveNotify(&globals.controlBar, leaveEv))
	{
	}
	else if (protocol_Xcb_hoverLeave(&globals.pxcb, leaveEv))
	{
	}
}

static void
gemcx_xcb_EventResponse__buttonPress(const xcb_generic_event_t *genericEvent)
{
	xcb_button_press_event_t *bp = (xcb_button_press_event_t *) genericEvent;
	static const uint32_t offsetSpeed = 40;

	switch (bp->detail)
	{
	case 1:	// Left-mouse click
	{
		bool connUrlRR = false;
		if (gemcx_xcb_ControlBar_buttonPress(&globals.controlBar,
					bp,
					&globals.event,
					&globals.historyStack,
					&connUrlRR,
					globals.urlStr))
		{
			if (connUrlRR)
			{
				gemcx_xcb_ConnectUrl_connectRender(
						&globals.connectUrl,
						globals.urlStr,
						false);
			}
			break;
		}

		for (uint32_t i = 0; i < globals.pxcb.links.length; ++i)
		{
			if (protocol_Links_clicked(&globals.pxcb.links, i, bp->event))
			{
				const struct protocol_Link *link = &globals.pxcb.links.links[i];
				printf("%d: ref: %s\n", i, link->ref);
				// TODO: Connect to new URL
				if (util_socket_urlHasScheme(link->ref, strlen(link->ref)))
				{
					strcpy(globals.urlStr, link->ref);
				}
				else
				{
					sprintf(globals.urlStr, "%s://%s%s%s%s",
							globals.client.host.scheme,
							globals.client.host.hostname,
							(globals.client.type == PROTOCOL_TYPE_GOPHER) ? "/1" : "",
							(link->ref[0] == '/') ? "" : "/",
							firstNonWhiteSpace(link->ref));
				}

				gemcx_xcb_ConnectUrl_connectRender(
						&globals.connectUrl,
						globals.urlStr,
						true);
				break;
			}
		}
	}	break;
	case 4:	// Scroll up
		globals.mainAreaYoffset -= offsetSpeed;
		if (globals.mainAreaYoffset <= 0)
		{
			globals.mainAreaYoffset = 0;
		}
		break;
	case 5:	// Scroll down
		globals.mainAreaYoffset += offsetSpeed;
		if (globals.mainAreaYoffset >= globals.mainAreaYMax)
		{
			globals.mainAreaYoffset = globals.mainAreaYMax;
		}
		break;
	default:
		break;
	}

	switch (bp->detail)
	{
	case 4:
	case 5:
		protocol_Xcb_offset(&globals.pxcb, 0, -globals.mainAreaYoffset);
		protocol_Xcb_scroll(&globals.pxcb, &globals.parser);
		if (bp->detail == 4)
		{
			// Rerender buttons: scroll up apparently
			// breaks rendering but not down
			protocol_Xcb_scroll(&globals.pxcb, &globals.parser);
		}

		ui_xcb_Pixmap_render(&globals.mainArea, 0, 0);
		ui_xcb_Pixmap_render(&globals.doubleBuffer, 0, 0);
		break;
	default:
		break;
	}
}

static void
gemcx_xcb_EventResponse__keyPress(const xcb_generic_event_t *genericEvent)
{
	xcb_key_press_event_t *prEv = (xcb_key_press_event_t *) genericEvent;

	static const uint32_t offsetSpeed = 40;
	static const uint32_t pageOffsetSpeed = 400;

	ui_xcb_Key_set(&globals.xkey, prEv->detail, prEv->state);
	//printf("key: %s\n", xkey.buffer);

	// COPY: Ctrl-c
	if ((globals.xkey.mask & XCB_KEY_BUT_MASK_CONTROL) &&
			(globals.xkey.keysymNoMask == XKB_KEY_c))
	{
		globals.clipboard.content = realloc(globals.clipboard.content,
				sizeof(char) * 16);
		strcpy(globals.clipboard.content, "TEST123");
		globals.clipboard.contentLength = strlen("TEST123");

		ui_xcb_Clipboard_selectionSetOwner(&globals.clipboard);
		return;
	}

	// PASTE: Ctrl-v
	if ((globals.xkey.mask & XCB_KEY_BUT_MASK_CONTROL) &&
			(globals.xkey.keysymNoMask == XKB_KEY_v))
	{
		ui_xcb_Clipboard_selectionCovert(&globals.clipboard);
		return;
	}

	ui_xcb_TextInput_modify(&globals.controlBar.urlInput, &globals.xkey);

	switch (globals.xkey.keysymNoMask)
	{
#if 0
	case XKB_KEY_Escape:
		ui_xcb_Event_close(&event);
		break;
#endif
	case XKB_KEY_Up:
	case XKB_KEY_Page_Up:
		globals.mainAreaYoffset -= (globals.xkey.keysymNoMask == XKB_KEY_Up) ? offsetSpeed : pageOffsetSpeed;
		if (globals.mainAreaYoffset <= 0)
		{
			globals.mainAreaYoffset = 0;
		}
		break;
	case XKB_KEY_Down:
	case XKB_KEY_Page_Down:
		globals.mainAreaYoffset += (globals.xkey.keysymNoMask == XKB_KEY_Down) ? offsetSpeed : pageOffsetSpeed;
		if (globals.mainAreaYoffset >= globals.mainAreaYMax)
		{
			globals.mainAreaYoffset = globals.mainAreaYMax;
		}
		break;
	case XKB_KEY_Return:
		if (globals.controlBar.urlInput.active)
		{
			printf("urlStr: %s\n", globals.urlStr);
			// Go to a new specified URL
			gemcx_xcb_ConnectUrl_connectRender(
					&globals.connectUrl,
					globals.urlStr,
					true);
		}
		break;
	default:
		break;
	}

	switch (globals.xkey.keysymNoMask)
	{
	case XKB_KEY_Up:
	case XKB_KEY_Page_Up:
	case XKB_KEY_Down:
	case XKB_KEY_Page_Down:
		protocol_Xcb_offset(&globals.pxcb, 0, -globals.mainAreaYoffset);
		protocol_Xcb_scroll(&globals.pxcb, &globals.parser);
		if (globals.xkey.keysymNoMask == XKB_KEY_Down ||
				globals.xkey.keysymNoMask == XKB_KEY_Page_Down)
		{
			// Rerender buttons: scroll up apparently
			// breaks rendering but not down
			protocol_Xcb_scroll(&globals.pxcb, &globals.parser);
		}

		ui_xcb_Pixmap_render(&globals.mainArea, 0, 0);
		ui_xcb_Pixmap_render(&globals.doubleBuffer, 0, 0);
		break;
	default:
		break;
	}
}

static void
gemcx_xcb_EventResponse__keyRelease(const xcb_generic_event_t *genericEvent)
{
	xcb_key_release_event_t *reEv = (xcb_key_release_event_t *) genericEvent;
	ui_xcb_Key_set(&globals.xkey, reEv->detail, reEv->state);
}

void
gemcx_xcb_EventResponse_response(const uint8_t responseType,
		const xcb_generic_event_t *genericEvent)
{
	static void (* const eventsResp[XCB_NO_OPERATION])(
			const xcb_generic_event_t *) = {
		[XCB_CLIENT_MESSAGE] = gemcx_xcb_EventResponse__clientMessage,
		[XCB_SELECTION_NOTIFY] = gemcx_xcb_EventResponse__selectionNotify,
		[XCB_SELECTION_REQUEST] = gemcx_xcb_EventResponse__selectionRequest,
		[XCB_PROPERTY_NOTIFY] = NULL,
		[XCB_SELECTION_CLEAR] = NULL,
		[XCB_MAP_NOTIFY] = gemcx_xcb_EventResponse__mapNotify,
		[XCB_CONFIGURE_NOTIFY] = gemcx_xcb_EventResponse__configureNotify,
		[XCB_EXPOSE] = gemcx_xcb_EventResponse__expose,
		[XCB_ENTER_NOTIFY] = gemcx_xcb_EventResponse__enterNotify,
		[XCB_LEAVE_NOTIFY] = gemcx_xcb_EventResponse__leaveNotify,
		[XCB_BUTTON_PRESS] = gemcx_xcb_EventResponse__buttonPress,
		[XCB_KEY_PRESS] = gemcx_xcb_EventResponse__keyPress,
		[XCB_KEY_RELEASE] = gemcx_xcb_EventResponse__keyRelease,
	};

	if (eventsResp[responseType] != NULL)
	{
		eventsResp[responseType](genericEvent);
	}
}

