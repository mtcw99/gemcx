#include "ui/xcb/context.h"

#include <stdlib.h>

void
ui_xcb_Context_init(struct ui_xcb_Context *context)
{
	context->connection = xcb_connect(NULL, NULL);
	context->screen = xcb_setup_roots_iterator(xcb_get_setup(context->connection)).data;
	context->depth = context->screen->root_depth;
	context->colormap = context->screen->default_colormap;

	/*
	 * Makes sure the windows gives an event signal during exit (so they can be closed properly)
	 */
	xcb_intern_atom_cookie_t atom_cookie_1 = xcb_intern_atom(
			context->connection,	// xcb_connection_t
			1,			// return valid atom id only if its exists
			12,			// length of name
			"WM_PROTOCOLS");	// name

	xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(
			context->connection,	// xcb_connection_t
			atom_cookie_1,		// atom cookie
			NULL);			// generic error pointer

	xcb_intern_atom_cookie_t atom_cookie_2 = xcb_intern_atom(
			context->connection,
			0,
			16,
			"WM_DELETE_WINDOW");

	context->exit_reply = xcb_intern_atom_reply(
			context->connection,
			atom_cookie_2,
			NULL);

	context->intern_atom_reply = reply;

	if (!ui_xcb_Key_init(&context->key, context->connection))
	{
		goto setup_destroy;
	}

	// Visual
	context->visual_type = NULL;
	xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(context->screen);
	for (; depth_iter.rem; xcb_depth_next(&depth_iter))
	{
		xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
		for (; visual_iter.rem; xcb_visualtype_next(&visual_iter))
		{
			if (context->screen->root_visual == visual_iter.data->visual_id)
			{
				context->visual_type = visual_iter.data;
				goto visual_found;
			}
		}
	}
visual_found:

	context->event = NULL;

	return;

setup_destroy:
	ui_xcb_Context_deinit(context);
	exit(-1);
}

void
ui_xcb_Context_deinit(struct ui_xcb_Context *context)
{
	free(context->exit_reply);
	free(context->intern_atom_reply);

	xcb_ungrab_pointer(context->connection, XCB_TIME_CURRENT_TIME);
	if (context->event != NULL)
	{
		free(context->event);
	}

	ui_xcb_Key_deinit(&context->key);

	xcb_flush(context->connection);
	if (context->connection)
	{
		xcb_disconnect(context->connection);
	}
}

