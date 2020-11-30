#ifndef UI_XCB_IMAGE_H
#define UI_XCB_IMAGE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <xcb/xcb.h>

#include "ui/xcb/context.h"

enum ui_xcb_Image_KeepAspect
{
	UI_XCB_IMAGE_KEEPASPECT_FALSE = 0,
	UI_XCB_IMAGE_KEEPASPECT_WIDTH,
	UI_XCB_IMAGE_KEEPASPECT_HEIGHT,

	UI_XCB_IMAGE_KEEPASPECT__TOTAL
};

struct ui_xcb_Image
{
	struct
	{
		uint8_t *data;
		int32_t width;
		int32_t height;
		int32_t n;
	} src;

	struct
	{
		uint8_t *data;
		xcb_rectangle_t rect;
	} dst;

	enum ui_xcb_Image_KeepAspect keepAspect;
	xcb_gcontext_t gc;
	xcb_pixmap_t pixmap;
	bool hasPixmapGc;

	xcb_drawable_t drawable;
	struct ui_xcb_Context *context;
};

void ui_xcb_Image_init(struct ui_xcb_Image *image,
		struct ui_xcb_Context *context,
		const xcb_drawable_t drawable);

void ui_xcb_Image_deinit(struct ui_xcb_Image *image);

void ui_xcb_Image_loadImage(struct ui_xcb_Image *image,
		const char *filename);

void ui_xcb_Image_resizeImage(struct ui_xcb_Image *image,
		const xcb_rectangle_t rect,
		const enum ui_xcb_Image_KeepAspect keepAspect);

void ui_xcb_Image_createPixmap(struct ui_xcb_Image *image);

void ui_xcb_Image_createResizedImagePixmap(struct ui_xcb_Image *image,
		const xcb_rectangle_t rect,
		const enum ui_xcb_Image_KeepAspect keepAspect);

void ui_xcb_Image_render(struct ui_xcb_Image *image);

#endif // UI_XCB_IMAGE_H

