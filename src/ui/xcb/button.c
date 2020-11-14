#include "ui/xcb/button.h"

#include "ui/xcb/cursor.h"

void
ui_xcb_Button_init(struct ui_xcb_Button *button,
		const char *str,
		struct ui_xcb_Text *font,
		struct ui_xcb_Context *context,
		const xcb_window_t parentWindow,
		const uint32_t backgroundColor,
		const uint32_t textColor,
		const uint32_t borderColor,
		const uint32_t borderWidth,
		const xcb_rectangle_t rect,
		const uint32_t textX,
		const uint32_t textY)
{
	ui_xcb_Subwindow_init(&button->subwindow,
			context, parentWindow,
			backgroundColor, borderColor,
			borderWidth, rect,
			XCB_EVENT_MASK_EXPOSURE |
			XCB_EVENT_MASK_BUTTON_PRESS |
			XCB_EVENT_MASK_BUTTON_RELEASE |
			XCB_EVENT_MASK_ENTER_WINDOW | 
			XCB_EVENT_MASK_LEAVE_WINDOW);
	ui_xcb_Subwindow_show(&button->subwindow, true);

	// Render text to pixmap
	ui_xcb_Pixmap_init(&button->pixmap, context, button->subwindow.id,
			rect.width, rect.height, backgroundColor);
	ui_xcb_Pixmap_clear(&button->pixmap);
	ui_xcb_Text_render(font, button->pixmap.pixmap, str,
			textX, textY, 
			textColor, 1.0);

	// Set pointer cursor to button subwindow
	ui_xcb_Cursor_set(context, button->subwindow.id, "pointer");

	button->text = font;
	button->context = context;
}

void
ui_xcb_Button_deinit(struct ui_xcb_Button *button)
{
	ui_xcb_Pixmap_deinit(&button->pixmap);
	ui_xcb_Subwindow_deinit(&button->subwindow);
	button->context = NULL;
}

void
ui_xcb_Button_setXY(struct ui_xcb_Button *button,
		const int16_t x, const int16_t y)
{
	button->subwindow.rect.x = x;
	button->subwindow.rect.y = y;
	ui_xcb_Subwindow_applyAttributes(&button->subwindow);
}

inline bool
ui_xcb_Button_pressed(struct ui_xcb_Button *button,
		const xcb_window_t eventWindow)
{
	return (button->subwindow.id == eventWindow);
}

void
ui_xcb_Button_render(struct ui_xcb_Button *button)
{
	ui_xcb_Pixmap_render(&button->pixmap, 0, 0);
}

void
ui_xcb_Button_show(struct ui_xcb_Button *button, const bool show)
{
	ui_xcb_Subwindow_show(&button->subwindow, show);
}

