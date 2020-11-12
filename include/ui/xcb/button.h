#ifndef UI_XCB_BUTTON_H
#define UI_XCB_BUTTON_H

#include <stdint.h>
#include <stdbool.h>
#include <xcb/xcb.h>

#include "ui/xcb/context.h"

struct ui_xcb_Button
{
	struct ui_xcb_Context *context;
};

void ui_xcb_Button_init(struct ui_xcb_Button *button,
		struct ui_xcb_Context *context);
void ui_xcb_Button_deinit(struct ui_xcb_Button *button);

#endif // UI_XCB_BUTTON_H

