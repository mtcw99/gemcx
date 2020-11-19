#ifndef GEMCX_XCB_EVENTRESPONSE_H
#define GEMCX_XCB_EVENTRESPONSE_H

#include <xcb/xcb.h>

void gemcx_xcb_EventResponse_response(const uint8_t responseType,
		const xcb_generic_event_t *genericEvent);

#endif // GEMCX_XCB_EVENTRESPONSE_H

