#ifndef PROTOCOL_XCB_H
#define PROTOCOL_XCB_H

#include <stdint.h>
#include <stdbool.h>

#include "protocol/type.h"
#include "protocol/parser.h"
#include "protocol/links.h"

#include "ui/xcb/pixmap.h"
#include "ui/xcb/text.h"
#include "ui/xcb/subwindow.h"

struct protocol_Xcb
{
	// Items
	struct ui_xcb_Pixmap pixmap;
	struct protocol_Links links;
	uint32_t offsetX;
	uint32_t offsetY;
	uint32_t paddingLeft;
	uint32_t paddingRight;
	bool hasItemsInit;

	// Just Pointers
	struct ui_xcb_Subwindow *subwindow;
	struct ui_xcb_Context *context;
	struct ui_xcb_Text *font;
};

void protocol_Xcb_init(struct protocol_Xcb *pgxcb,
		struct ui_xcb_Context *context,
		struct ui_xcb_Text *font,
		const xcb_drawable_t drawable,
		struct ui_xcb_Subwindow *subwindow,
		const uint32_t width,
		const uint32_t height,
		const uint32_t backgroundColor);

void protocol_Xcb_deinit(struct protocol_Xcb *pgxcb);

void protocol_Xcb_itemsInit(struct protocol_Xcb *pgxcb,
		struct protocol_Parser *parser);

uint32_t protocol_Xcb_render(struct protocol_Xcb *pgxcb,
		const struct protocol_Parser *parser);

void protocol_Xcb_scroll(struct protocol_Xcb *pgxcb,
		const struct protocol_Parser *parser);

void protocol_Xcb_offset(struct protocol_Xcb *pgxcb,
		const uint32_t offsetX, const uint32_t offsetY);

void protocol_Xcb_padding(struct protocol_Xcb *pgxcb,
		const uint32_t left, const uint32_t right);

bool protocol_Xcb_hoverEnter(struct protocol_Xcb *pgxcb,
		const xcb_enter_notify_event_t *const restrict enterEv);

bool protocol_Xcb_hoverLeave(struct protocol_Xcb *pgxcb,
		const xcb_leave_notify_event_t *const restrict leaveEv);

#endif // PROTOCOL_XCB_H

