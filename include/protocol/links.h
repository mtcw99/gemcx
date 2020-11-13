#ifndef PROTOCOL_LINKS_H
#define PROTOCOL_LINKS_H

#include <stdint.h>
#include <stdbool.h>

#include "ui/xcb/button.h"

struct protocol_Link
{
	char *str;
	char *ref;
	struct ui_xcb_Button button;
};

struct protocol_Links
{
	struct protocol_Link *links;
	uint32_t allocate;
	uint32_t length;

	uint32_t backgroundColor;

	// Just Pointers
	struct ui_xcb_Context *context;
	struct ui_xcb_Text *font;
};

void protocol_Links_init(struct protocol_Links *links,
		struct ui_xcb_Context *context,
		struct ui_xcb_Text *font,
		const uint32_t backgroundColor);

void protocol_Links_deinit(struct protocol_Links *links);
uint32_t protocol_Links_new(struct protocol_Links *links,
		const char *str,
		const char *ref,
		const xcb_window_t parentWindow);

bool protocol_Links_clicked(struct protocol_Links *links,
		const uint32_t index,
		const xcb_window_t eventWindow);

void protocol_Links_render(struct protocol_Links *links,
		const uint32_t index,
		const int16_t x, const int16_t y);

#endif // PROTOCOL_LINKS_H

