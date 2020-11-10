#include "ui/xcb/event.h"

#include <stdlib.h>

void
ui_xcb_Event_init(struct ui_xcb_Event *event,
		xcb_connection_t *connection)
{
	event->generic_event = NULL;
	event->connection = connection;
	event->response_type = 0;
	event->close = false;
}

void
ui_xcb_Event_deinit(struct ui_xcb_Event *event)
{
	if (event->generic_event != NULL)
	{
		free(event->generic_event);
		event->generic_event = NULL;
	}
	event->connection = NULL;
	event->response_type = 0;
	event->close = true;
}

bool
ui_xcb_Event_waitForEvent(struct ui_xcb_Event *event)
{
	if (event->generic_event != NULL)
	{
		free(event->generic_event);
		xcb_flush(event->connection);
		event->generic_event = NULL;
	}
	if (event->close)
	{
		return false;
	}
	event->generic_event = xcb_wait_for_event(event->connection);
	event->response_type = event->generic_event->response_type & ~0x80;
	return true;
}

void
ui_xcb_Event_close(struct ui_xcb_Event *event)
{
	event->close = true;
}

