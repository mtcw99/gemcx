#ifndef PROTOCOL_XCB_H
#define PROTOCOL_XCB_H

#include <stdint.h>
#include <stdbool.h>

#include "protocol/type.h"
#include "protocol/parser.h"
#include "ui/xcb/pixmap.h"
#include "ui/xcb/text.h"

uint32_t protocol_Xcb_render(const struct protocol_Parser *parser,
		struct ui_xcb_Pixmap* pixmap,
		struct ui_xcb_Text *text,
		const uint32_t width,
		const uint32_t height,
		const uint32_t yOffset);

#endif // PROTOCOL_XCB_H

