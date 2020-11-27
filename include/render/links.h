#ifndef RENDER_LINKS_H
#define RENDER_LINKS_H

#include <stdint.h>
#include <stdbool.h>

#include "ui/xcb/button.h"

struct render_Link
{
	char *str;
	char *ref;
	struct ui_xcb_Button button;
};

struct render_Links
{
	struct render_Link *links;
	uint32_t allocate;
	uint32_t length;

	uint32_t backgroundColor;

	// Just Pointers
	struct ui_xcb_Context *context;
	struct ui_xcb_Text *font;
};

void render_Links_init(struct render_Links *links,
		struct ui_xcb_Context *context,
		struct ui_xcb_Text *font,
		const uint32_t backgroundColor);

void render_Links_deinit(struct render_Links *links);
void render_Links_clear(struct render_Links *links);

uint32_t render_Links_new(struct render_Links *links,
		const char *str,
		const char *ref,
		const xcb_window_t parentWindow);

bool render_Links_clicked(struct render_Links *links,
		const uint32_t index,
		const xcb_window_t eventWindow);

void render_Links_render(struct render_Links *links,
		const uint32_t index,
		const int16_t x, const int16_t y,
		const uint32_t width, const uint32_t height);

bool render_Links_hoverEnter(struct render_Links *links,
		const xcb_enter_notify_event_t *const restrict enterEv);

bool render_Links_hoverLeave(struct render_Links *links,
		const xcb_leave_notify_event_t *const restrict leaveEv);

#endif // RENDER_LINKS_H

