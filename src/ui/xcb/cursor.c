#include "ui/xcb/cursor.h"

#include <xcb/xcb_cursor.h>

void
ui_xcb_Cursor_set(struct ui_xcb_Context *context,
		const xcb_window_t windowId,
		const char *name)
{
	xcb_cursor_context_t *cursorContext;
	xcb_cursor_context_new(context->connection, context->screen, &cursorContext);

	xcb_cursor_t cursor = xcb_cursor_load_cursor(cursorContext, name);
	if (cursor != XCB_CURSOR_NONE)
	{
		xcb_change_window_attributes(context->connection,
				windowId,
				XCB_CW_CURSOR,
				&cursor);
	}
	else
	{
		fprintf(stderr, "ui/xcb/Cursor: ERROR: Cannot get cursor"
				" '%s'\n",
				name);
	}

	xcb_cursor_context_free(cursorContext);
}

