#ifndef UI_XCB_CLIPBOARD_H
#define UI_XCB_CLIPBOARD_H

#include <xcb/xcb.h>
#include "ui/xcb/context.h"

enum ui_xcb_Clipboard_ia
{
	UI_XCB_CLIPBOARD_IA_INCR = 0,
	UI_XCB_CLIPBOARD_IA_CLIPBOARD,
	UI_XCB_CLIPBOARD_IA_UTF8STRING,
	UI_XCB_CLIPBOARD_IA_XCLIPD,
	UI_XCB_CLIPBOARD_IA_XSEL_DATA,

	UI_XCB_CLIPBOARD_IA__TOTAL
};

struct ui_xcb_Clipboard
{
	xcb_intern_atom_reply_t *replies[UI_XCB_CLIPBOARD_IA__TOTAL];
	xcb_window_t selectionNotifyWindow;
	xcb_window_t selectionRequestWindow;

	char *content;
	uint32_t contentLength;

	struct ui_xcb_Context *context;
};

void ui_xcb_Clipboard_init(struct ui_xcb_Clipboard * const restrict clipboard,
		struct ui_xcb_Context * const context);

void ui_xcb_Clipboard_deinit(struct ui_xcb_Clipboard * const restrict clipboard);

void ui_xcb_Clipboard_selectionNotify(struct ui_xcb_Clipboard * const restrict clipboard,
		const xcb_selection_notify_event_t * const selNotifyEv);

void ui_xcb_Clipboard_selectionRequest(struct ui_xcb_Clipboard * const restrict clipboard,
		const xcb_selection_request_event_t * const selReqEv);

void ui_xcb_Clipboard_selectionCovert(struct ui_xcb_Clipboard * const restrict clipboard);
void ui_xcb_Clipboard_selectionSetOwner(struct ui_xcb_Clipboard * const restrict clipboard);
void ui_xcb_Clipboard_selectionClear(struct ui_xcb_Clipboard * const restrict clipboard);

#endif // UI_XCB_CLIPBOARD_H

