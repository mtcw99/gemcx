#ifndef RENDER_FORMAT_TEXT_XCB_H
#define RENDER_FORMAT_TEXT_XCB_H

#include <stdint.h>
#include <stdbool.h>

#include "parser/format/text.h"
#include "render/xcb.h"

uint32_t render_format_text_Xcb_render(struct render_Xcb *pgxcb,
		const struct parser_format_Text *fText,
		const bool scroll);

#endif // RENDER_FORMAT_TEXT_XCB_H

