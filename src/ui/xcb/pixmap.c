#include "ui/xcb/pixmap.h"

void
ui_xcb_Pixmap_init(struct ui_xcb_Pixmap *pixmap,
		struct ui_xcb_Context *context,
		const xcb_drawable_t drawable,
		const uint32_t width,
		const uint32_t height,
		const uint32_t backgroundColor)
{
	pixmap->gc = xcb_generate_id(context->connection);
	pixmap->pixmap = xcb_generate_id(context->connection);
	pixmap->rect = (xcb_rectangle_t) {
		.x = 0, .y = 0,
		.width = width, .height = height
	};

	xcb_create_pixmap(context->connection,
			context->depth,
			pixmap->pixmap,
			drawable,
			pixmap->rect.width,
			pixmap->rect.height);

	xcb_create_gc(context->connection,
			pixmap->gc,
			pixmap->pixmap,
			XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES,
			(uint32_t [2]) {
				[0] = backgroundColor,
				[1] = 0
			});

	pixmap->rootDrawable = drawable;
	pixmap->context = context;

	ui_xcb_Pixmap_clear(pixmap);
}

void
ui_xcb_Pixmap_deinit(struct ui_xcb_Pixmap *pixmap)
{
	xcb_free_gc(pixmap->context->connection,
			pixmap->gc);
	xcb_free_pixmap(pixmap->context->connection,
			pixmap->pixmap);
	pixmap->rect.width = 0;
	pixmap->rect.height = 0;
	pixmap->rootDrawable = 0;
	pixmap->context = NULL;
}

void
ui_xcb_Pixmap_render(struct ui_xcb_Pixmap *pixmap,
		const int16_t x,
		const int16_t y)
{
	xcb_copy_area(pixmap->context->connection,
			pixmap->pixmap,
			pixmap->rootDrawable,
			pixmap->gc,
			0, 0,
			x, y,
			pixmap->rect.width, pixmap->rect.height);

}

void
ui_xcb_Pixmap_clear(struct ui_xcb_Pixmap *pixmap)
{
	xcb_poly_fill_rectangle(pixmap->context->connection,
			pixmap->pixmap,
			pixmap->gc,
			1,
			&pixmap->rect);
}

