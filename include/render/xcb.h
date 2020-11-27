#ifndef RENDER_XCB_H
#define RENDER_XCB_H

#include <stdint.h>
#include <stdbool.h>

#include "parser/generic.h"

#include "render/links.h"

#include "ui/xcb/pixmap.h"
#include "ui/xcb/text.h"
#include "ui/xcb/subwindow.h"

struct render_Xcb
{
	// Items
	struct ui_xcb_Pixmap pixmap;
	struct render_Links links;
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

void render_Xcb_init(struct render_Xcb *pgxcb,
		struct ui_xcb_Context *context,
		struct ui_xcb_Text *font,
		const xcb_drawable_t drawable,
		struct ui_xcb_Subwindow *subwindow,
		const uint32_t width,
		const uint32_t height,
		const uint32_t backgroundColor);

void render_Xcb_deinit(struct render_Xcb *pgxcb);

void render_Xcb_itemsInit(struct render_Xcb *pgxcb,
		struct Parser *parser);

uint32_t render_Xcb_render(struct render_Xcb *pgxcb,
		const struct Parser *parser);

void render_Xcb_scroll(struct render_Xcb *pgxcb,
		const struct Parser *parser);

void render_Xcb_offset(struct render_Xcb *pgxcb,
		const uint32_t offsetX, const uint32_t offsetY);

void render_Xcb_padding(struct render_Xcb *pgxcb,
		const uint32_t left, const uint32_t right);

bool render_Xcb_hoverEnter(struct render_Xcb *pgxcb,
		const xcb_enter_notify_event_t *const restrict enterEv);

bool render_Xcb_hoverLeave(struct render_Xcb *pgxcb,
		const xcb_leave_notify_event_t *const restrict leaveEv);

#endif // RENDER_XCB_H

