#ifndef PROTOCOL_GOPHER_XCB_H
#define PROTOCOL_GOPHER_XCB_H

#include <stdint.h>
#include <stdbool.h>

#include "parser/format/gopher.h"
#include "render/xcb.h"

uint32_t render_format_gopher_Xcb_render(struct render_Xcb *pgxcb,
		const struct parser_format_Gopher *parser,
		const bool scroll);

void render_format_gopher_Xcb_itemsInit(struct render_Xcb *pgxcb,
		struct parser_format_Gopher *parser);

#endif // PROTOCOL_GOPHER_XCB_H

