#ifndef GEMINI_XCB_H
#define GEMINI_XCB_H

#include <stdint.h>
#include <stdbool.h>

#include "gemini/parser.h"
#include "ui/xcb/pixmap.h"
#include "ui/xcb/text.h"

uint32_t gemini_Xcb_render(struct ui_xcb_Pixmap *mainArea,
		struct ui_xcb_Text *text,
		const struct gemini_Parser *parser,
		const uint32_t width,
		const uint32_t height,
		const uint32_t yOffset);

#endif // GEMINI_XCB_H

