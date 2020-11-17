#include "protocol/links.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util/memory.h"

enum
{
	PROTOCOL_LINK_ALLOC = 16
};

void
protocol_Links_init(struct protocol_Links *links,
		struct ui_xcb_Context *context,
		struct ui_xcb_Text *font,
		const uint32_t backgroundColor)
{
	links->links = NULL;
	links->allocate = 0;
	links->length = 0;

	links->backgroundColor = backgroundColor;

	links->context = context;
	links->font = font;
}

void
protocol_Links_deinit(struct protocol_Links *links)
{
	if ((links->allocate > 0) && (links->links != NULL))
	{
		for (uint32_t i = 0; i < links->length; ++i)
		{
			struct protocol_Link *link = &links->links[i];
			ui_xcb_Button_deinit(&link->button);
			util_memory_free(link->str);
			util_memory_free(link->ref);
		}
		util_memory_free(links->links);
	}
	links->links = NULL;
	links->allocate = 0;
	links->length = 0;
}

static void
protocol_Links__expand(struct protocol_Links *links)
{
	if (links->links == NULL)
	{
		links->allocate = PROTOCOL_LINK_ALLOC;
		links->links = util_memory_calloc(
				sizeof(struct protocol_Link), links->allocate);
		links->length = 0;
	}
	else if (links->allocate >= (links->length - 1))
	{
		links->allocate += PROTOCOL_LINK_ALLOC;
		links->links = util_memory_realloc(links->links,
				sizeof(struct protocol_Link) * links->allocate);
	}
}

uint32_t
protocol_Links_new(struct protocol_Links *links,
		const char *str,
		const char *ref,
		const xcb_window_t parentWindow)
{
	protocol_Links__expand(links);

	const uint32_t index = links->length++;
	struct protocol_Link *link = &links->links[index];

	link->str = util_memory_calloc(sizeof(char), strlen(str) + 1);
	strcpy(link->str, str);

	link->ref = util_memory_calloc(sizeof(char), strlen(ref) + 1);
	strcpy(link->ref, ref);

	memset(&link->button, 0, sizeof(struct ui_xcb_Button));
	const uint32_t calcStrWidth = ui_xcb_Text_calcWidth(links->font, str);
	ui_xcb_Button_init(&link->button,
			link->str,
			links->font,
			links->context,
			parentWindow,
			links->backgroundColor,
			0x0088CC,	// Text color
			0x000000, 0,
			(const xcb_rectangle_t) { 0, 0, calcStrWidth, 20 },
			0, 0,
			links->backgroundColor,
			0x00FFFF);	// Text hover color
	ui_xcb_Button_show(&link->button, false);

	return index;
}

bool
protocol_Links_clicked(struct protocol_Links *links,
		const uint32_t index,
		const xcb_window_t eventWindow)
{
	assert(index < links->length);
	return ui_xcb_Button_pressed(&links->links[index].button, eventWindow);
}

void
protocol_Links_render(struct protocol_Links *links,
		const uint32_t index,
		const int16_t x, const int16_t y,
		const uint32_t width, const uint32_t height)
{
	struct protocol_Link *link = &links->links[index];
	if ((x > 0) && (y > 0) && (x < width) && (y < height))
	{
		//printf("SHOW: %d %d %d\n", index, x, y);
		ui_xcb_Button_show(&link->button, true);
		ui_xcb_Button_setXY(&link->button, x, y);
		ui_xcb_Button_render(&link->button);
	}
	else
	{
		ui_xcb_Button_show(&link->button, false);
	}
}

bool
protocol_Links_hoverEnter(struct protocol_Links *links,
		const xcb_enter_notify_event_t *const restrict enterEv)
{
	for (uint32_t i = 0; i < links->length; ++i)
	{
		if (ui_xcb_Button_hoverEnter(&links->links[i].button, enterEv))
		{
			return true;
		}
	}
	return false;
}

bool
protocol_Links_hoverLeave(struct protocol_Links *links,
		const xcb_leave_notify_event_t *const restrict leaveEv)
{
	for (uint32_t i = 0; i < links->length; ++i)
	{
		if (ui_xcb_Button_hoverLeave(&links->links[i].button, leaveEv))
		{
			return true;
		}
	}
	return false;
}

