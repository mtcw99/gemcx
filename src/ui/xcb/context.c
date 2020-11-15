#include "ui/xcb/context.h"

#include <stdlib.h>

void
ui_xcb_Context__randr(struct ui_xcb_Context *context)
{
	// Request for screen resource to X server
	xcb_randr_get_screen_resources_cookie_t screenResCookie =
			xcb_randr_get_screen_resources(
				context->connection, context->screen->root);

	// Recieve reply from X server
	xcb_randr_get_screen_resources_reply_t *screenResReply =
			xcb_randr_get_screen_resources_reply(
				context->connection, screenResCookie, NULL);

	if (!screenResReply)
	{
		fprintf(stderr, "ui/xcb/Context::randr: ERROR no reply"
				" from screen\n");
		return;
	}

	xcb_randr_query_version_cookie_t verCookie =
			xcb_randr_query_version(context->connection,
				XCB_RANDR_MAJOR_VERSION, XCB_RANDR_MINOR_VERSION);
	xcb_randr_query_version_reply_t *verReply =
			xcb_randr_query_version_reply(context->connection,
				verCookie, NULL);
	if (!verReply)
	{
		fprintf(stderr, "ui/xcb/Context::randr: ERROR on getting"
				" version reply\n");
	}
	else
	{
		free(verReply);
		verReply = NULL;
	}

	const uint32_t crtcLen = xcb_randr_get_screen_resources_crtcs_length(screenResReply);
	xcb_randr_crtc_t *crtcFirst = xcb_randr_get_screen_resources_crtcs(screenResReply);

	// Send request to the X server about crtc informations
	xcb_randr_get_crtc_info_cookie_t crtcResCookie[crtcLen];
	for (uint32_t i = 0; i < crtcLen; ++i)
	{
		crtcResCookie[i] = xcb_randr_get_crtc_info(
				context->connection, *(crtcFirst + i), 0);
	}

	// Recieve reply from the X server about crtc informations
	xcb_randr_get_crtc_info_reply_t *crtcResReply[crtcLen];
	for (uint32_t i = 0; i < crtcLen; ++i)
	{
		crtcResReply[i] = xcb_randr_get_crtc_info_reply(
				context->connection, crtcResCookie[i], 0);
	}

	for (uint32_t i = 0; i < crtcLen; ++i)
	{
		const xcb_randr_get_crtc_info_reply_t *crtcInfo = crtcResReply[i];
		if (crtcInfo && !((0 == crtcInfo->x)
					&& (0 == crtcInfo->y)
					&& (0 == crtcInfo->width)
					&& (0 == crtcInfo->height)))
		{
			ui_xcb_Monitors_add(&context->monitors, crtcInfo);
		}

		free(crtcResReply[i]);
		crtcResReply[i] = NULL;
	}

	free(screenResReply);
	screenResReply = NULL;
}

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

	ui_xcb_Monitors_init(&context->monitors);
	ui_xcb_Context__randr(context);

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

	ui_xcb_Monitors_deinit(&context->monitors);

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

void
ui_xcb_Context_getRootWH(struct ui_xcb_Context *context)
{
	// context->screen->root
	// get window info???
}

