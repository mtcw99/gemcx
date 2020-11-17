#ifndef UI_XCB_BUTTON_H
#define UI_XCB_BUTTON_H

#include <stdint.h>
#include <stdbool.h>
#include <xcb/xcb.h>

#include "ui/xcb/context.h"
#include "ui/xcb/subwindow.h"
#include "ui/xcb/text.h"
#include "ui/xcb/pixmap.h"

struct ui_xcb_Button
{
	struct ui_xcb_Subwindow subwindow;

	struct ui_xcb_Pixmap pixmap;
	struct ui_xcb_Pixmap hoverPixmap;

	bool hover;

	struct ui_xcb_Text *text;
	struct ui_xcb_Context *context;
};

void ui_xcb_Button_init(struct ui_xcb_Button *button,
		const char *str,
		struct ui_xcb_Text *font,
		struct ui_xcb_Context *context,
		const xcb_window_t parentWindow,
		const uint32_t backgroundColor,
		const uint32_t textColor,
		const uint32_t borderColor,
		const uint32_t borderWidth,
		const xcb_rectangle_t rect,
		const uint32_t textX,
		const uint32_t textY,
		const uint32_t hoverBackgroundColor,
		const uint32_t hoverTextColor);

void ui_xcb_Button_deinit(struct ui_xcb_Button *button);
void ui_xcb_Button_setXY(struct ui_xcb_Button *button,
		const int16_t x, const int16_t y);
bool ui_xcb_Button_pressed(struct ui_xcb_Button *button,
		const xcb_window_t eventWindow);
void ui_xcb_Button_render(struct ui_xcb_Button *button);
void ui_xcb_Button_show(struct ui_xcb_Button *button, const bool show);
bool ui_xcb_Button_hoverEnter(struct ui_xcb_Button *button,
		const xcb_enter_notify_event_t *const enterEv);
bool ui_xcb_Button_hoverLeave(struct ui_xcb_Button *button,
		const xcb_leave_notify_event_t *const leaveEv);

#endif // UI_XCB_BUTTON_H

