#ifndef UI_XCB_PIXMAP_H
#define UI_XCB_PIXMAP_H

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>

#include "ui/xcb/context.h"
#include "ui/xcb/window.h"

struct ui_xcb_Pixmap
{
	xcb_gcontext_t	gc;
	xcb_pixmap_t	pixmap;
	xcb_rectangle_t	rect;
	xcb_drawable_t	rootDrawable;

	struct ui_xcb_Context *context;
};

void ui_xcb_Pixmap_init(struct ui_xcb_Pixmap *pixmap,
		struct ui_xcb_Context *context,
		const xcb_drawable_t drawable,
		const uint32_t width,
		const uint32_t height,
		const uint32_t backgroundColor);
void ui_xcb_Pixmap_deinit(struct ui_xcb_Pixmap *pixmap);
void ui_xcb_Pixmap_render(struct ui_xcb_Pixmap *pixmap,
		const int16_t x,
		const int16_t y);
void ui_xcb_Pixmap_clear(struct ui_xcb_Pixmap *pixmap);

#endif // UI_XCB_PIXMAP_H

