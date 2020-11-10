#ifndef UI_XCB_EVENT_H
#define UI_XCB_EVENT_H

#include <xcb/xcb.h>
#include <stdint.h>
#include <stdbool.h>

struct ui_xcb_Event
{
	xcb_generic_event_t *generic_event;
	xcb_connection_t *connection;
	uint8_t response_type;
	bool close;

	//xcb_window_t window;
};

void ui_xcb_Event_init(struct ui_xcb_Event *event,
		xcb_connection_t *connection);
void ui_xcb_Event_deinit(struct ui_xcb_Event *event);
bool ui_xcb_Event_waitForEvent(struct ui_xcb_Event *event);
void ui_xcb_Event_close(struct ui_xcb_Event *event);

#endif // UI_XCB_EVENT_H

