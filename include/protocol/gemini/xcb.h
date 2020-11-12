#ifndef PROTOCOL_GEMINI_XCB_H
#define PROTOCOL_GEMINI_XCB_H

#include <stdint.h>
#include <stdbool.h>

#include "protocol/gemini/parser.h"
#include "ui/xcb/pixmap.h"
#include "ui/xcb/text.h"

uint32_t p_gemini_Xcb_render(const struct p_gemini_Parser *parser,
		struct ui_xcb_Pixmap *pixmap,
		struct ui_xcb_Text *text,
		const uint32_t width,
		const uint32_t height,
		const uint32_t yOffset);

#endif // PROTOCOL_GEMINI_XCB_H

