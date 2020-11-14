#ifndef UI_XCB_CURSOR_H
#define UI_XCB_CURSOR_H

#include <stdint.h>
#include <stdbool.h>

#include "ui/xcb/context.h"
#include "ui/xcb/window.h"

void ui_xcb_Cursor_set(struct ui_xcb_Context *context,
		const xcb_window_t windowId,
		const char *name);

#endif // UI_XCB_CURSOR_H

