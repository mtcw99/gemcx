#ifndef UI_XCB_SUBWINDOW_H
#define UI_XCB_SUBWINDOW_H

#include <stdint.h>
#include <stdbool.h>

#include <xcb/xcb.h>

#include "ui/xcb/context.h"

struct ui_xcb_Subwindow
{
	xcb_window_t id;
	xcb_window_t parentId;

	// Attributes
	uint32_t backgroundColor;
	uint32_t borderColor;
	xcb_rectangle_t rect;
	uint32_t borderWidth;

	struct ui_xcb_Context *context;
};

bool ui_xcb_Subwindow_init(struct ui_xcb_Subwindow *subwindow,
		struct ui_xcb_Context *context,
		const xcb_window_t parentWindow,
		const uint32_t backgroundColor,
		const uint32_t borderColor,
		const uint32_t borderWidth,
		const xcb_rectangle_t rect,
		const uint32_t eventMask);

void ui_xcb_Subwindow_deinit(struct ui_xcb_Subwindow *subwindow);
void ui_xcb_Subwindow_applyAttributes(struct ui_xcb_Subwindow *subwindow);

#endif // UI_XCB_SUBWINDOW_H

