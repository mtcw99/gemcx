#ifndef GEMCX_XCB_CONTROLBAR_H
#define GEMCX_XCB_CONTROLBAR_H

#include <stdint.h>
#include <stdbool.h>

#include <xcb/xcb.h>

#include "protocol/historyStack.h"

#include "ui/xcb/context.h"
#include "ui/xcb/subwindow.h"
#include "ui/xcb/button.h"
#include "ui/xcb/textInput.h"
#include "ui/xcb/menu.h"
#include "ui/xcb/event.h"
#include "ui/xcb/clipboard.h"

struct gemcx_xcb_ControlBar
{
	struct ui_xcb_Subwindow subwindow;
	struct ui_xcb_Button buttonMenu;
	struct ui_xcb_Button buttonBack;
	struct ui_xcb_Button buttonForward;
	struct ui_xcb_TextInput urlInput;
	struct ui_xcb_Menu menu;

	uint32_t urlInputXOffset;
};

enum gemcx_xcb_controlBar_Menu
{
	GEMCX_XCB_CONTROLBAR_MENU_SETTINGS = 0,
	GEMCX_XCB_CONTROLBAR_MENU_EXIT,

	GEMCX_XCB_CONTROLBAR_MENU__TOTAL
};

void gemcx_xcb_ControlBar_init(struct gemcx_xcb_ControlBar *controlBar,
		struct ui_xcb_Context *context,
		const xcb_window_t windowId,
		const uint32_t barHeight,
		char *urlStr,
		const uint32_t urlStrLen,
		struct ui_xcb_Text *fonts);

void gemcx_xcb_ControlBar_deinit(struct gemcx_xcb_ControlBar *controlBar);

void gemcx_xcb_ControlBar_configureNotify(struct gemcx_xcb_ControlBar *controlBar,
		const xcb_configure_notify_event_t *cnEvent);

void gemcx_xcb_ControlBar_expose(struct gemcx_xcb_ControlBar *controlBar,
		const xcb_expose_event_t *exEv);

bool gemcx_xcb_ControlBar_enterNotify(struct gemcx_xcb_ControlBar *controlBar,
		const xcb_enter_notify_event_t *enterEv);

bool gemcx_xcb_ControlBar_leaveNotify(struct gemcx_xcb_ControlBar *controlBar,
		const xcb_leave_notify_event_t *leaveEv);

bool gemcx_xcb_ControlBar_buttonPress(struct gemcx_xcb_ControlBar *controlBar,
		const xcb_button_press_event_t *bpEv,
		struct ui_xcb_Event *event,
		struct protocol_HistoryStack *historyStack,
		bool *connUrlRR,
		char *urlStr);

void gemcx_xcb_ControlBar_selectionNotify(struct gemcx_xcb_ControlBar *controlBar,
		struct ui_xcb_Clipboard *clipboard);

#endif // GEMCX_XCB_CONTROLBAR_H

