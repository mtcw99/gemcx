#include "ui/xcb/window.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui/xcb/windowShared.h"

bool
ui_xcb_Window_init(struct ui_xcb_Window *window,
		struct ui_xcb_Context *context,
		const char *name)
{
	strcpy(window->name, name);

	bool created = false;
	window->id = ui_xcb_WindowShared_createWindow(context,
			context->screen->root,
			0x000000,
			0x000000,
			0,
			(const xcb_rectangle_t) { 0, 0, 640, 480 },
			UI_XCB_WINDOWSHARED_ROOTMASK,
			&created);
	if (!created)
	{
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
	window->width = 640;
	window->height = 480;
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

void
ui_xcb_Window_updateInfo(struct ui_xcb_Window *window,
		const uint32_t width,
		const uint32_t height)
{
	window->width = width;
	window->height = height;
}

void
ui_xcb_Window_minSize(struct ui_xcb_Window *window,
		const uint32_t width,
		const uint32_t height)
{
	xcb_size_hints_t hints;
	xcb_icccm_size_hints_set_min_size(&hints, width, height);
	xcb_icccm_size_hints_set_max_size(&hints, 1920, 1080);	// TODO get from context
	xcb_icccm_set_wm_size_hints(window->context->connection,
			window->id,
			XCB_ATOM_WM_NORMAL_HINTS,
			&hints);
}

