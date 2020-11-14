#ifndef PROTOCOL_XCB_H
#define PROTOCOL_XCB_H

#include <stdint.h>
#include <stdbool.h>

#include "protocol/type.h"
#include "protocol/parser.h"
#include "protocol/links.h"

#include "ui/xcb/pixmap.h"
#include "ui/xcb/text.h"

struct protocol_Xcb
{
	enum protocol_Type type;

	// Items
	struct ui_xcb_Pixmap pixmap;
	struct protocol_Links links;
	uint32_t offsetX;
	uint32_t offsetY;

	// Just Pointers
	struct ui_xcb_Window *window;
	struct ui_xcb_Context *context;
	struct ui_xcb_Text *font;
};

void protocol_Xcb_init(struct protocol_Xcb *pgxcb,
		struct ui_xcb_Context *context,
		struct ui_xcb_Text *font,
		const xcb_drawable_t drawable,
		struct ui_xcb_Window *window,
		const uint32_t width,
		const uint32_t height,
		const uint32_t backgroundColor,
		const enum protocol_Type type);

void protocol_Xcb_deinit(struct protocol_Xcb *pgxcb);

void protocol_Xcb_itemsInit(struct protocol_Xcb *pgxcb,
		struct protocol_Parser *parser);

uint32_t protocol_Xcb_render(struct protocol_Xcb *pgxcb,
		const struct protocol_Parser *parser,
		const uint32_t width,
		const uint32_t height);

void protocol_Xcb_scroll(struct protocol_Xcb *pgxcb,
		const struct protocol_Parser *parser);

void protocol_Xcb_offset(struct protocol_Xcb *pgxcb,
		const uint32_t offsetX, const uint32_t offsetY);

#endif // PROTOCOL_XCB_H

