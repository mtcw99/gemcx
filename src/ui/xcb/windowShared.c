#include "ui/xcb/windowShared.h"

#include <stdio.h>

const uint32_t UI_XCB_WINDOWSHARED_ROOTMASK =
		XCB_EVENT_MASK_EXPOSURE		|
		XCB_EVENT_MASK_BUTTON_PRESS	|
		XCB_EVENT_MASK_BUTTON_RELEASE	|
		XCB_EVENT_MASK_POINTER_MOTION	|
		XCB_EVENT_MASK_ENTER_WINDOW	|
		XCB_EVENT_MASK_LEAVE_WINDOW	|
		XCB_EVENT_MASK_KEY_PRESS	|
		XCB_EVENT_MASK_KEY_RELEASE	|
		XCB_EVENT_MASK_STRUCTURE_NOTIFY	|
		XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;

inline xcb_window_t
ui_xcb_WindowShared_createWindow(struct ui_xcb_Context *context,
		const xcb_window_t parentWindow,
		const uint32_t backgroundColor,
		const uint32_t borderColor,
		const uint32_t borderWidth,
		const xcb_rectangle_t rect,
		const uint32_t eventMask,
		bool *created)
{
	xcb_window_t newWindow = xcb_generate_id(context->connection);

	xcb_void_cookie_t cookie = xcb_create_window_checked(
			context->connection,
			context->depth,
			newWindow,
			parentWindow,
			rect.x, rect.y,
			rect.width, rect.height,
			borderWidth,
			XCB_WINDOW_CLASS_INPUT_OUTPUT,
			context->screen->root_visual,
			XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK,
			&(uint32_t [3]) {
				[0] = backgroundColor,
				[1] = borderColor,
				[2] = eventMask
			});

	xcb_generic_error_t *error = xcb_request_check(
			context->connection,
			cookie);

	if (error)
	{
		fprintf(stderr, "UI/XCB/WindowShared: ERROR: Cannot create a new"
				" window: XCB Error code: %d\n",
				error->error_code);
	}

	*created = (!error);
	return newWindow;
}

