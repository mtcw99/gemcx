#ifndef UI_XCB_MENU_H
#define UI_XCB_MENU_H

#include "ui/xcb/context.h"
#include "ui/xcb/text.h"
#include "ui/xcb/button.h"
#include "ui/xcb/subwindow.h"

struct ui_xcb_Menu
{
	// Buttons array
	struct ui_xcb_Button *buttons;
	uint32_t buttonsLength;
	uint32_t buttonsAlloc;

	struct ui_xcb_Subwindow subwindow;

	// Styling information
	struct ui_xcb_MenuStyle
	{
		uint32_t backgroundColor;
		uint32_t textColor;
		uint32_t textX;
		uint32_t textY;
		xcb_rectangle_t subWindowRect;
		uint32_t buttonHeight;
	} style;

	uint32_t buttonY;
	bool visible;

	// Pointers to others
	struct ui_xcb_Text *font;
	struct ui_xcb_Context *context;
};

void ui_xcb_Menu_init(struct ui_xcb_Menu *const restrict menu,
		struct ui_xcb_Context *const restrict context,
		struct ui_xcb_Text *const restrict font,
		const xcb_window_t parentWindow,
		const struct ui_xcb_MenuStyle style);

void ui_xcb_Menu_deinit(struct ui_xcb_Menu *const restrict menu);

void ui_xcb_Menu_add(struct ui_xcb_Menu *const restrict menu,
		const char *const restrict label);

void ui_xcb_Menu_show(struct ui_xcb_Menu *const restrict menu,
		const bool show);

void ui_xcb_Menu_render(struct ui_xcb_Menu *const restrict menu);

// -2 - Pressed on the subwindow
// -1 - Not pressed at all
// 0 to menu->buttonsLength-1 - Pressed at one of the buttons
int32_t ui_xcb_Menu_pressed(struct ui_xcb_Menu *const restrict menu,
		const xcb_button_press_event_t *const restrict bpEv);

#endif // UI_XCB_MENU_H

