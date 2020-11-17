#include "ui/xcb/clipboard.h"

#include <stdlib.h>
#include <string.h>

#include <xcb/xcb_atom.h>
#include <xcb/xcb_event.h>
#include <xcb/xcb_icccm.h>

#include <time.h>

/*
 * CREDIT:
 * - xcb selection/clipboard goes to:
 *   https://github.com/awesomeWM/awesome/blob/master/selection.c
 * - bit of info on how it works:
 *   https://www.uninformativ.de/blog/postings/2017-04-02/0/POSTING-en.html
 */

static const struct ui_xcb_Clipboard_ia_infos
{
	const char *nameStr;
	const uint32_t nameLength;
} uiXcbClipboardIAInfo[UI_XCB_CLIPBOARD_IA__TOTAL] = {
	[UI_XCB_CLIPBOARD_IA_INCR] 	= { "INCR", 4 },
	[UI_XCB_CLIPBOARD_IA_CLIPBOARD] = { "CLIPBOARD", 9 },
	[UI_XCB_CLIPBOARD_IA_UTF8STRING]= { "UTF8_STRING", 11 },
	[UI_XCB_CLIPBOARD_IA_XCLIPD] 	= { "XCLIPD", 6 },
	[UI_XCB_CLIPBOARD_IA_XSEL_DATA]	= { "XSEL_DATA", 9 }
};

void
ui_xcb_Clipboard_init(struct ui_xcb_Clipboard * const restrict clipboard,
		struct ui_xcb_Context * const context)
{
	xcb_intern_atom_cookie_t cookies[UI_XCB_CLIPBOARD_IA__TOTAL] = { 0 };

	for (uint32_t i = 0; i < UI_XCB_CLIPBOARD_IA__TOTAL; ++i)
	{
		cookies[i] = xcb_intern_atom(
				context->connection,
				1,
				uiXcbClipboardIAInfo[i].nameLength,
				uiXcbClipboardIAInfo[i].nameStr);
	}

	for (uint32_t i = 0; i < UI_XCB_CLIPBOARD_IA__TOTAL; ++i)
	{
		clipboard->replies[i] = xcb_intern_atom_reply(
				context->connection,
				cookies[i],
				NULL);
	}

	// Create selection notify window
	clipboard->selectionNotifyWindow = xcb_generate_id(context->connection);
	xcb_create_window(context->connection,
			context->screen->root_depth,
			clipboard->selectionNotifyWindow,
			context->screen->root,
			0, 0, 1, 1, 0,
			XCB_COPY_FROM_PARENT,
			context->screen->root_visual,
			XCB_CW_BACK_PIXEL | XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK,
			(const uint32_t [3]) {
				context->screen->black_pixel,
				1,
				XCB_EVENT_MASK_PROPERTY_CHANGE
			});

	// Create selection request window
	clipboard->selectionRequestWindow = xcb_generate_id(context->connection);
	xcb_create_window(context->connection,
			context->screen->root_depth,
			clipboard->selectionRequestWindow,
			context->screen->root,
			0, 0, 1, 1, 0,
			XCB_COPY_FROM_PARENT,
			context->screen->root_visual,
			XCB_CW_BACK_PIXEL | XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK,
			(const uint32_t [3]) {
				context->screen->black_pixel,
				1,
				XCB_EVENT_MASK_PROPERTY_CHANGE
			});

	clipboard->content = calloc(sizeof(char), 64);
	clipboard->contentLength = 0;

	clipboard->context = context;

	//ui_xcb_Clipboard_selectionCovert(clipboard);
}

void
ui_xcb_Clipboard_deinit(struct ui_xcb_Clipboard * const restrict clipboard)
{
	xcb_destroy_window(clipboard->context->connection,
			clipboard->selectionRequestWindow);
	xcb_destroy_window(clipboard->context->connection,
			clipboard->selectionNotifyWindow);

	if (clipboard->content != NULL)
	{
		free(clipboard->content);
		clipboard->content = NULL;
	}
	clipboard->contentLength = 0;

	for (uint32_t i = 0; i < UI_XCB_CLIPBOARD_IA__TOTAL; ++i)
	{
		if (clipboard->replies[i] != NULL)
		{
			free(clipboard->replies[i]);
		}
	}
}

void
ui_xcb_Clipboard_selectionNotify(struct ui_xcb_Clipboard * const restrict clipboard,
		const xcb_selection_notify_event_t * const selNotifyEv)
{
	if (selNotifyEv->selection == clipboard->replies[UI_XCB_CLIPBOARD_IA_CLIPBOARD]->atom &&
			selNotifyEv->property != XCB_NONE)
	{
		xcb_get_property_cookie_t cookie =
			xcb_icccm_get_text_property(clipboard->context->connection,
					selNotifyEv->requestor,
					selNotifyEv->property);

		xcb_icccm_get_text_property_reply_t prop = { 0 };
		if (xcb_icccm_get_text_property_reply(clipboard->context->connection,
					cookie, &prop, NULL))
		{
			//printf("String (%d): %s\n", prop.name_len, prop.name);
			clipboard->content = realloc(clipboard->content,
					sizeof(char) * prop.name_len + 1);
			strcpy(clipboard->content, prop.name);
			clipboard->contentLength = prop.name_len;

			xcb_icccm_get_text_property_reply_wipe(&prop);
			xcb_delete_property(clipboard->context->connection,
					selNotifyEv->requestor,
					selNotifyEv->property);
		}
	}
}

void
ui_xcb_Clipboard_selectionRequest(struct ui_xcb_Clipboard * const restrict clipboard,
		const xcb_selection_request_event_t * const selReqEv)
{
	if (selReqEv->target != clipboard->replies[UI_XCB_CLIPBOARD_IA_UTF8STRING]->atom ||
			selReqEv->property == XCB_NONE)
	{
		xcb_selection_notify_event_t selNotifyEv = { 0 };

		selNotifyEv.response_type = XCB_SELECTION_NOTIFY;
		selNotifyEv.requestor = selReqEv->requestor;
		selNotifyEv.selection = selReqEv->selection;
		selNotifyEv.target = selReqEv->target;
		selNotifyEv.property = XCB_NONE;
		selNotifyEv.time = selReqEv->time;

		xcb_send_event(clipboard->context->connection,
				true, selReqEv->requestor, XCB_EVENT_MASK_NO_EVENT,
				(char *) &selNotifyEv);
		return;
	}

	//printf("ui_xcb_Clipboard_selectionRequest\n");
	xcb_change_property(clipboard->context->connection,
			XCB_PROP_MODE_REPLACE,
			selReqEv->requestor,
			selReqEv->property,
			clipboard->replies[UI_XCB_CLIPBOARD_IA_UTF8STRING]->atom,
			8,
			clipboard->contentLength,
			clipboard->content);

	xcb_change_window_attributes(clipboard->context->connection,
			selReqEv->requestor,
			XCB_CW_EVENT_MASK,
			(uint32_t []) { 0 });

	xcb_selection_notify_event_t selNotifyEv = { 0 };
	selNotifyEv.response_type = XCB_SELECTION_NOTIFY;
	selNotifyEv.requestor = selReqEv->requestor;
	selNotifyEv.selection = selReqEv->selection;
	selNotifyEv.target = selReqEv->target;
	selNotifyEv.property = selReqEv->property;
	selNotifyEv.time = selReqEv->time;

	xcb_send_event(clipboard->context->connection,
			true, selReqEv->requestor, XCB_EVENT_MASK_NO_EVENT,
			(char *) &selNotifyEv);

	xcb_flush(clipboard->context->connection);
}

void
ui_xcb_Clipboard_selectionCovert(struct ui_xcb_Clipboard * const restrict clipboard)
{
	xcb_convert_selection(clipboard->context->connection,
			clipboard->selectionNotifyWindow,
			clipboard->replies[UI_XCB_CLIPBOARD_IA_CLIPBOARD]->atom,
			clipboard->replies[UI_XCB_CLIPBOARD_IA_UTF8STRING]->atom,
			clipboard->replies[UI_XCB_CLIPBOARD_IA_XSEL_DATA]->atom,
			XCB_CURRENT_TIME);

	// XCB_ATOM_PRIMARY - primary clipboard selection
	// UI_XCB_CLIPBOARD_IA_CLIPBOARD - Ctrl-C clipboard selection
}

void
ui_xcb_Clipboard_selectionSetOwner(struct ui_xcb_Clipboard * const restrict clipboard)
{
	xcb_set_selection_owner(clipboard->context->connection,
			clipboard->selectionRequestWindow,
			clipboard->replies[UI_XCB_CLIPBOARD_IA_CLIPBOARD]->atom,
			XCB_CURRENT_TIME);
}

void
ui_xcb_Clipboard_selectionClear(struct ui_xcb_Clipboard * const restrict clipboard)
{
	xcb_set_selection_owner(clipboard->context->connection,
			XCB_NONE,
			clipboard->replies[UI_XCB_CLIPBOARD_IA_CLIPBOARD]->atom,
			XCB_CURRENT_TIME);
}

