#ifndef UI_XCB_CONTEXT_H
#define UI_XCB_CONTEXT_H

#include <stdint.h>
#include <stdbool.h>

#include <xcb/xcb.h>
#include <xcb/randr.h>

#include "ui/xcb/key.h"
#include "ui/xcb/monitors.h"

struct ui_xcb_Context
{
	xcb_connection_t *	connection;
	xcb_screen_t *		screen;
	int32_t			depth;
	xcb_colormap_t		colormap;
	xcb_generic_event_t *	event;
	xcb_intern_atom_reply_t *intern_atom_reply;
	xcb_intern_atom_reply_t *exit_reply;
	xcb_visualtype_t *	visual_type;

	struct ui_xcb_Key	key;
	struct ui_xcb_Monitors	monitors;

#if 0
	xcb_void_cookie_t 	cookie;
	xcb_colormap_t		colormap;
	xcb_generic_event_t *	event;
	int32_t			depth;
#endif
};

void ui_xcb_Context_init(struct ui_xcb_Context *context);
void ui_xcb_Context_deinit(struct ui_xcb_Context *context);

void ui_xcb_Context_getRootWH(struct ui_xcb_Context *context);

#endif // UI_XCB_CONTEXT_H

