#ifndef PROTOCOL_GEMINI_XCB_H
#define PROTOCOL_GEMINI_XCB_H

#include <stdint.h>
#include <stdbool.h>

#include "parser/format/gemini.h"
#include "render/xcb.h"

uint32_t render_format_gemini_Xcb_render(struct render_Xcb *pgxcb,
		const struct parser_format_Gemini *parser,
		const bool scroll);

void render_format_gemini_Xcb_itemsInit(struct render_Xcb *pgxcb,
		struct parser_format_Gemini *parser);

#endif // PROTOCOL_GEMINI_XCB_H

