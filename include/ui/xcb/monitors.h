#ifndef UI_XCB_MONITORS_H
#define UI_XCB_MONITORS_H

#include <xcb/xcb.h>
#include <xcb/randr.h>

struct ui_xcb_Monitor
{
	xcb_randr_mode_t mode;
	int16_t x;
	int16_t y;
	uint16_t width;
	uint16_t height;
};

struct ui_xcb_Monitors
{
	struct ui_xcb_Monitor *monitors;
	uint32_t alloc;
	uint32_t length;
};

void ui_xcb_Monitors_init(struct ui_xcb_Monitors *monitors);
void ui_xcb_Monitors_deinit(struct ui_xcb_Monitors *monitors);

void ui_xcb_Monitors_add(struct ui_xcb_Monitors *monitors,
		const xcb_randr_get_crtc_info_reply_t *reply);

#endif // UI_XCB_MONITORS_H

