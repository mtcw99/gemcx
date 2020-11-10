#include "ui/xcb/window.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool
ui_xcb_Window_init(struct ui_xcb_Window *window,
		struct ui_xcb_Context *context,
		const char *name)
{
	strcpy(window->name, name);
	window->id = xcb_generate_id(context->connection);

	xcb_void_cookie_t cookie = xcb_create_window_checked(
			context->connection,
			context->depth,
			window->id,
			context->screen->root,
			0, 0,		// x, y
			640, 480,	// w, h
			0,
			XCB_WINDOW_CLASS_INPUT_OUTPUT,
			context->screen->root_visual,
			XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK,
			&(uint32_t [2]) {
				[0] = 0x000000,
				[1] = XCB_EVENT_MASK_EXPOSURE	|
				XCB_EVENT_MASK_BUTTON_PRESS	|
				XCB_EVENT_MASK_BUTTON_RELEASE	|
				XCB_EVENT_MASK_POINTER_MOTION	|
				XCB_EVENT_MASK_ENTER_WINDOW	|
				XCB_EVENT_MASK_LEAVE_WINDOW	|
				XCB_EVENT_MASK_KEY_PRESS	|
				XCB_EVENT_MASK_KEY_RELEASE	|
				XCB_EVENT_MASK_STRUCTURE_NOTIFY	|
				XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
			});

	xcb_generic_error_t *error = xcb_request_check(
			context->connection,
			cookie);

	if (error)
	{
		fprintf(stderr, "UI/XCB/Window: ERROR: Cannot create a new"
				" window: XCB Error code: %d\n",
				error->error_code);
		return false;
	}

	// Setting window name
	xcb_change_property(context->connection,
			XCB_PROP_MODE_REPLACE,
			window->id,
			XCB_ATOM_WM_NAME,
			XCB_ATOM_STRING,
			8,
			strlen(name),
			name);

	// Making sure the window can close via WM
	xcb_change_property(context->connection,
			XCB_PROP_MODE_REPLACE,
			window->id,
			context->intern_atom_reply->atom,
			4,
			32,
			1,
			&context->exit_reply->atom);

	// Graphic context
	window->gc = xcb_generate_id(context->connection);
	xcb_create_gc(context->connection,
			window->gc,
			window->id,
			XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES,
			&(uint32_t [2]) {
				context->screen->black_pixel,
				0
			});

	window->context = context;
	return true;
}

void
ui_xcb_Window_deinit(struct ui_xcb_Window *window)
{
	xcb_unmap_window(window->context->connection, window->id);
	xcb_destroy_window(window->context->connection, window->id);
}

void
ui_xcb_Window_map(struct ui_xcb_Window *window, const bool map)
{
	static xcb_void_cookie_t (* const xcb_mapunmap_win[2])(
			xcb_connection_t *,
			xcb_window_t) = {
		[false] = xcb_unmap_window,
		[true] = xcb_map_window
	};
	xcb_mapunmap_win[map](window->context->connection, window->id);
}

