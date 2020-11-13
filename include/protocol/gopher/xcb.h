#ifndef PROTOCOL_GOPHER_XCB_H
#define PROTOCOL_GOPHER_XCB_H

#include <stdint.h>
#include <stdbool.h>

#include "protocol/gopher/parser.h"
#include "protocol/xcb.h"

uint32_t p_gopher_Xcb_render(struct protocol_Xcb *pgxcb,
		const struct p_gopher_Parser *parser);

void p_gopher_Xcb_itemsInit(struct protocol_Xcb *pgxcb,
		struct p_gopher_Parser *parser);

#endif // PROTOCOL_GOPHER_XCB_H

