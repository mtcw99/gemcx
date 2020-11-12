#ifndef PROTOCOL_GOPHER_XCB_H
#define PROTOCOL_GOPHER_XCB_H

#include <stdint.h>
#include <stdbool.h>

#include "protocol/gopher/parser.h"
#include "ui/xcb/pixmap.h"
#include "ui/xcb/text.h"

uint32_t p_gopher_Xcb_render(const struct p_gopher_Parser *parser,
		struct ui_xcb_Pixmap *pixmap,
		struct ui_xcb_Text *text);

#endif // PROTOCOL_GOPHER_XCB_H

