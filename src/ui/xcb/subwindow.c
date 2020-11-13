#include "ui/xcb/subwindow.h"

#include "ui/xcb/windowShared.h"

bool
ui_xcb_Subwindow_init(struct ui_xcb_Subwindow *subwindow,
		struct ui_xcb_Context *context,
		const xcb_window_t parentWindow,
		const uint32_t backgroundColor,
		const uint32_t borderColor,
		const uint32_t borderWidth,
		const xcb_rectangle_t rect,
		const uint32_t eventMask)
{
	bool created = false;
	subwindow->id = ui_xcb_WindowShared_createWindow(context,
			parentWindow,
			backgroundColor,
			borderColor,
			borderWidth,
			rect,
			eventMask,
			&created);

	subwindow->parentId = parentWindow;
	subwindow->backgroundColor = backgroundColor;
	subwindow->borderColor = borderColor;
	subwindow->rect = rect;
	subwindow->borderWidth = borderWidth;
	subwindow->context = context;
	return created;
}

void
ui_xcb_Subwindow_deinit(struct ui_xcb_Subwindow *subwindow)
{
	xcb_unmap_window(subwindow->context->connection, subwindow->id);
	xcb_destroy_window(subwindow->context->connection, subwindow->id);
}

void
ui_xcb_Subwindow_applyAttributes(struct ui_xcb_Subwindow *subwindow)
{
	xcb_change_window_attributes(subwindow->context->connection,
			subwindow->id,
			XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL,
			(uint32_t [2]) {
				[0] = subwindow->backgroundColor,
				[1] = subwindow->borderColor
			});

	xcb_configure_window(subwindow->context->connection,
			subwindow->id,
			XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
			XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT |
			XCB_CONFIG_WINDOW_BORDER_WIDTH,
			(uint32_t [5]) {
				[0] = subwindow->rect.x,
				[1] = subwindow->rect.y,
				[2] = subwindow->rect.width,
				[3] = subwindow->rect.height,
				[4] = subwindow->borderWidth
			});
}

void
ui_xcb_Subwindow_show(struct ui_xcb_Subwindow *subwindow,
		const bool show)
{
	static xcb_void_cookie_t (* const xcb_mapunmap_win[2])(
			xcb_connection_t *,
			xcb_window_t) = {
		[false] = xcb_unmap_window,
		[true] = xcb_map_window
	};
	xcb_mapunmap_win[show](subwindow->context->connection, subwindow->id);
}

