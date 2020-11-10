#ifndef UI_XCB_KEY_H
#define UI_XCB_KEY_H

#include <stdbool.h>
#include <stdint.h>
#include <xkbcommon/xkbcommon-x11.h>

struct ui_xcb_Key
{
	xkb_keysym_t		keysym;
	xkb_keysym_t 		keysym_no_mask;
	uint32_t 		mask;
	char 			buffer[64];

	uint8_t			kb_first_event;
	struct xkb_context *	kb_ctx;
	int32_t			kb_device_id;
	struct xkb_keymap *	kb_keymap;
	struct xkb_state *	kb_state;
	struct xkb_state *	kb_dummy_state;
};

bool ui_xcb_Key_init(struct ui_xcb_Key *key,
		xcb_connection_t *connection);

void ui_xcb_Key_deinit(struct ui_xcb_Key *key);

#endif // UI_XCB_KEY_H

