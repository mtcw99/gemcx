#ifndef UI_XCB_TEXTINPUT_H
#define UI_XCB_TEXTINPUT_H

#include <stdint.h>
#include <stdbool.h>

#include "ui/xcb/context.h"
#include "ui/xcb/subwindow.h"
#include "ui/xcb/text.h"
#include "ui/xcb/pixmap.h"
#include "ui/xcb/key.h"

struct ui_xcb_TextInput
{
	char *str;
	uint32_t strMax;
	uint32_t strLen;

	struct ui_xcb_Subwindow subwindow;
	struct ui_xcb_Pixmap pixmap;

	uint32_t textColor;
	uint32_t renderWidth;
	uint32_t textX;
	uint32_t textY;

	bool active;

	struct ui_xcb_Text *font;
	struct ui_xcb_Context *context;
};

void ui_xcb_TextInput_init(struct ui_xcb_TextInput * const textinput,
		struct ui_xcb_Context * const context,
		struct ui_xcb_Text * const font,
		const xcb_window_t parentWindow,
		const xcb_rectangle_t rect,
		const uint32_t backgroundColor,
		const uint32_t textColor,
		const uint32_t textX,
		const uint32_t textY,
		char * const str,
		const uint32_t strMax);

void ui_xcb_TextInput_deinit(struct ui_xcb_TextInput * const textinput);
void ui_xcb_TextInput_render(struct ui_xcb_TextInput * const textinput);
void ui_xcb_TextInput_modify(struct ui_xcb_TextInput * const textinput,
		const struct ui_xcb_Key * const xkey);
void ui_xcb_TextInput_append(struct ui_xcb_TextInput * const textinput,
		const char *str,
		const uint32_t strLength);

#endif // UI_XCB_TEXTINPUT_H

