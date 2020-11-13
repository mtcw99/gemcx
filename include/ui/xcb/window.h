#ifndef UI_XCB_WINDOW_H
#define UI_XCB_WINDOW_H

#include <stdint.h>
#include <stdbool.h>

#include <xcb/xcb.h>

#include "ui/xcb/context.h"

struct ui_xcb_Window
{
	xcb_window_t	id;
	char		name[64];
	uint32_t	mask;
	bool		shown;
	xcb_gcontext_t	gc;

	xcb_window_t	*subs_id;
	uint32_t	length;
	uint32_t	alloc;

	uint32_t	width;
	uint32_t	height;

	bool		mapInit;

	struct ui_xcb_Context *context;
};

bool ui_xcb_Window_init(struct ui_xcb_Window *windoww,
		struct ui_xcb_Context *context,
		const char *name);

void ui_xcb_Window_deinit(struct ui_xcb_Window *window);
void ui_xcb_Window_map(struct ui_xcb_Window *window, const bool map);
void ui_xcb_Window_updateInfo(struct ui_xcb_Window *window,
		const uint32_t width,
		const uint32_t height);

#endif // UI_XCB_WINDOW_H

