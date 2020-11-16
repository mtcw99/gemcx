#include "ui/xcb/monitors.h"

#include "util/memory.h"
#include <stdio.h>

enum
{
	MONITORS_ALLOC = 4
};

void
ui_xcb_Monitors_init(struct ui_xcb_Monitors *monitors)
{
	monitors->monitors = NULL;
	monitors->alloc = 0;
	monitors->length = 0;
}

void
ui_xcb_Monitors_deinit(struct ui_xcb_Monitors *monitors)
{
	if (monitors->monitors != NULL)
	{
		util_memory_free(monitors->monitors);
	}

	monitors->alloc = 0;
	monitors->length = 0;
}

static void
ui_xcb_Monitors__expand(struct ui_xcb_Monitors *monitors)
{
	if (monitors->monitors == NULL)
	{
		monitors->alloc = MONITORS_ALLOC;
		monitors->monitors = util_memory_calloc(
				sizeof(struct ui_xcb_Monitor), monitors->alloc);
	}
	else
	{
		monitors->alloc += MONITORS_ALLOC;
		monitors->monitors = util_memory_realloc(monitors->monitors,
				sizeof(struct ui_xcb_Monitor) * monitors->alloc);
	}
}

void
ui_xcb_Monitors_add(struct ui_xcb_Monitors *monitors,
		const xcb_randr_get_crtc_info_reply_t *reply)
{
	ui_xcb_Monitors__expand(monitors);

	struct ui_xcb_Monitor *monitor = &monitors->monitors[monitors->length++];
	monitor->mode = reply->mode;
	monitor->x = reply->x;
	monitor->y = reply->y;
	monitor->width = reply->width;
	monitor->height = reply->height;
}

