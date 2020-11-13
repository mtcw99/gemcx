#ifndef PROTOCOL_GEMINI_XCB_H
#define PROTOCOL_GEMINI_XCB_H

#include <stdint.h>
#include <stdbool.h>

#include "protocol/gemini/parser.h"
#include "protocol/xcb.h"

uint32_t p_gemini_Xcb_render(struct protocol_Xcb *pgxcb,
		const struct p_gemini_Parser *parser,
		const uint32_t width,
		const uint32_t height);

void p_gemini_Xcb_itemsInit(struct protocol_Xcb *pgxcb,
		const struct p_gemini_Parser *parser);

#endif // PROTOCOL_GEMINI_XCB_H

