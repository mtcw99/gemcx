#include "render/links.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util/memory.h"
#include "util/ex.h"

enum
{
	PROTOCOL_LINK_ALLOC = 16
};

void
render_Links_init(struct render_Links *links,
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
render_Links_deinit(struct render_Links *links)
{
	if (links->links != NULL)
	{
		for (uint32_t i = 0; i < links->length; ++i)
		{
			struct render_Link *link = &links->links[i];
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

void
render_Links_clear(struct render_Links *links)
{
	render_Links_deinit(links);
}

static void
render_Links__expand(struct render_Links *links)
{
	if (links->links == NULL)
	{
		links->allocate = PROTOCOL_LINK_ALLOC;
		links->links = util_memory_calloc(
				sizeof(struct render_Link), links->allocate);
		links->length = 0;
	}
	else if (links->allocate >= (links->length - 1))
	{
		links->allocate += PROTOCOL_LINK_ALLOC;
		links->links = util_memory_realloc(links->links,
				sizeof(struct render_Link) * links->allocate);
	}
}

uint32_t
render_Links_new(struct render_Links *links,
		const char *str,
		const char *ref,
		const xcb_window_t parentWindow)
{
	render_Links__expand(links);

	const uint32_t index = links->length++;
	struct render_Link *link = &links->links[index];
	const uint32_t strSize = strlen(str);

	if ((strlen(ref) > 7) && (!strncmp(ref, "http://", 7) ||
				!strncmp(ref, "https://", 8)))
	{
		// Its an HTTP(S) URL
		char strTmp[1024] = { 0 };
		strcpy(strTmp, str);
		link->str = util_memory_calloc(sizeof(char), strSize + 11);
		util_ex_rmchs(strTmp, strSize, "\n", true);
		sprintf(link->str, "%s [%s] ",
				strTmp,
				(!strncmp(ref, "http://", 7)) ? "HTTP" : "HTTPS");
	}
#if 0
	else if ((strlen(ref) > 9) && (!strncmp(ref, "gopher://", 9)))
	{
		// Its a gopher URL
	}
#endif
	else
	{
		link->str = util_memory_calloc(sizeof(char), strSize + 1);
		strcpy(link->str, str);
	}

	link->ref = util_memory_calloc(sizeof(char), strlen(ref) + 1);
	strcpy(link->ref, ref);

	memset(&link->button, 0, sizeof(struct ui_xcb_Button));
	const uint32_t calcStrWidth = ui_xcb_Text_calcWidth(links->font, link->str);
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
render_Links_clicked(struct render_Links *links,
		const uint32_t index,
		const xcb_window_t eventWindow)
{
	assert(index < links->length);
	return ui_xcb_Button_pressed(&links->links[index].button, eventWindow);
}

void
render_Links_render(struct render_Links *links,
		const uint32_t index,
		const int16_t x, const int16_t y,
		const uint32_t width, const uint32_t height)
{
	struct render_Link *link = &links->links[index];
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
render_Links_hoverEnter(struct render_Links *links,
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
render_Links_hoverLeave(struct render_Links *links,
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

