#ifndef GOPHER_XCB_H
#define GOPHER_XCB_H

#include <stdint.h>
#include <stdbool.h>

#include "gopher/parser.h"
#include "ui/xcb/pixmap.h"
#include "ui/xcb/text.h"

uint32_t gopher_Xcb_render(const struct gopher_Parser *parser,
		struct ui_xcb_Pixmap *pixmap,
		struct ui_xcb_Text *text);

#endif // GOPHER_XCB_H

