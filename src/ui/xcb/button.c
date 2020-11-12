#include "ui/xcb/button.h"

void
ui_xcb_Button_init(struct ui_xcb_Button *button,
		struct ui_xcb_Context *context)
{
	button->context = context;
}

void
ui_xcb_Button_deinit(struct ui_xcb_Button *button)
{
	button->context = NULL;
}

