#ifndef UI_XCB_WINDOWSHARED_H
#define UI_XCB_WINDOWSHARED_H

#include <stdint.h>
#include <stdbool.h>
#include <xcb/xcb.h>

#include "ui/xcb/context.h"

extern const uint32_t UI_XCB_WINDOWSHARED_ROOTMASK;

/*
 * PARAMETERS:
 * 	*context
 * 	parentWindow - If root window, use: context->screen->root
 * 	backgroundColor
 * 	borderColor
 * 	borderWidth
 * 	rect
 * 	eventMask - If root window, use: UI_XCB_WINDOWSHARED_ROOTMASK
 * 	*created
 *
 * RETURNS: New window ID
 * 	IMPORTANT: MUST NOT DISCARD RETURN VALUE
 */
xcb_window_t ui_xcb_WindowShared_createWindow(struct ui_xcb_Context *context,
		const xcb_window_t parentWindow,
		const uint32_t backgroundColor,
		const uint32_t borderColor,
		const uint32_t borderWidth,
		const xcb_rectangle_t rect,
		const uint32_t eventMask,
		bool *created);

#endif // UI_XCB_WINDOWSHARED_H

