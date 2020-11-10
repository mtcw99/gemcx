#include "ui/xcb/key.h"

#include <stdio.h>

bool
ui_xcb_Key_init(struct ui_xcb_Key *key,
		xcb_connection_t *connection)
{
	int32_t xkbc_setup_ret = xkb_x11_setup_xkb_extension(connection,
			XKB_X11_MIN_MAJOR_XKB_VERSION,
			XKB_X11_MIN_MINOR_XKB_VERSION,
			XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS,
			NULL, NULL, &key->kb_first_event, NULL);

	if (!xkbc_setup_ret)
	{
		fprintf(stderr, "Cannot setup XKB extension.\n");
		return false;
	}

	key->kb_ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	if (!key->kb_ctx)
	{
		fprintf(stderr, "ERROR: xkb_context_new ERROR\n");
		return false;
	}

	key->kb_device_id = xkb_x11_get_core_keyboard_device_id(connection);
	if (key->kb_device_id == -1)
	{
		fprintf(stderr, "ERROR: xkb_x11_get_core_keyboard_device_id ERROR\n");
		return false;
	}

	key->kb_keymap = xkb_x11_keymap_new_from_device(key->kb_ctx,
			connection, key->kb_device_id,
			XKB_MAP_COMPILE_NO_FLAGS);
	if (!key->kb_keymap)
	{
		fprintf(stderr, "ERROR: xkb_x11_keymap_new_from_device ERROR\n");
		return false;
	}

	key->kb_state = xkb_x11_state_new_from_device(key->kb_keymap,
			connection, key->kb_device_id);
	if (!key->kb_state)
	{
		fprintf(stderr, "ERROR: xkb_x11_state_new_from_device ERROR\n");
		return false;
	}

	key->kb_dummy_state = xkb_x11_state_new_from_device(key->kb_keymap,
			connection, key->kb_device_id);
	if (!key->kb_dummy_state)
	{
		fprintf(stderr, "ERROR: xkb_x11_state_new_from_device ERROR\n");
		return false;
	}

	return true;
}

void
ui_xcb_Key_deinit(struct ui_xcb_Key *key)
{
	if (key->kb_state)		xkb_state_unref(key->kb_state);
	if (key->kb_dummy_state)	xkb_state_unref(key->kb_dummy_state);
	if (key->kb_keymap)		xkb_keymap_unref(key->kb_keymap);
	if (key->kb_ctx)		xkb_context_unref(key->kb_ctx);
}

