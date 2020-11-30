#include "ui/xcb/image.h"

#include "stb_image.h"
#include "stb_image_resize.h"

#include "util/memory.h"

#include <xcb/xcb_image.h>

static uint32_t
ui_xcb_Image__colorsBlendAlpha(const uint32_t colora,
		const uint32_t colorb,
		const uint32_t alpha)
{
	const uint32_t rb1 = ((0x100 - alpha) * (colora & 0xFF00FF)) >> 8;
	const uint32_t rb2 = (alpha * (colorb & 0xFF00FF)) >> 8;
	const uint32_t g1  = ((0x100 - alpha) * (colora & 0x00FF00)) >> 8;
	const uint32_t g2  = (alpha * (colorb & 0x00FF00)) >> 8;
	return ((rb1 | rb2) & 0xFF00FF) + ((g1 | g2) & 0x00FF00);
}

static void
ui_xcb_Image__destroyPixmapGc(struct ui_xcb_Image *image)
{
	xcb_free_pixmap(image->context->connection, image->pixmap);
	xcb_free_gc(image->context->connection, image->gc);
	image->pixmap = 0;
	image->gc = 0;
}

void
ui_xcb_Image_init(struct ui_xcb_Image *image,
		struct ui_xcb_Context *context,
		const xcb_drawable_t drawable)
{
	image->src.data = NULL;
	image->dst.data = NULL;
	image->drawable = drawable;
	image->context = context;
	image->hasPixmapGc = false;
}

void
ui_xcb_Image_deinit(struct ui_xcb_Image *image)
{
	ui_xcb_Image__destroyPixmapGc(image);

	if (image->src.data != NULL)
	{
		stbi_image_free(image->src.data);
	}
	if (image->dst.data != NULL)
	{
		printf("image->dst.data: FREE ON: %p\n", image->dst.data);
		util_memory_free(image->dst.data);
		image->dst.data = NULL;
	}

	image->context = NULL;
}

void
ui_xcb_Image_loadImage(struct ui_xcb_Image *image,
		const char *filename)
{
	image->src.data = stbi_load(filename,
			&image->src.width,
			&image->src.height,
			&image->src.n,
			4);
}

void
ui_xcb_Image_resizeImage(struct ui_xcb_Image *image,
		const xcb_rectangle_t rect,
		const enum ui_xcb_Image_KeepAspect keepAspect)
{
	static const uint32_t backgroundColor = 0x000000;
	image->dst.rect = rect;

	switch (keepAspect)
	{
	case UI_XCB_IMAGE_KEEPASPECT_WIDTH:
		image->dst.rect.height = (uint32_t) (
				((double) image->src.height / (double) image->src.width) *
				(double) image->dst.rect.width);
		break;
	case UI_XCB_IMAGE_KEEPASPECT_HEIGHT:
		image->dst.rect.width = (uint32_t) (
				((double) image->src.width / (double) image->src.height) *
				(double) image->dst.rect.height);
		break;
	case UI_XCB_IMAGE_KEEPASPECT_FALSE:
	default:
		break;
	}

	if (image->dst.data != NULL)
	{
#ifdef DEBUG
		printf("image->dst.data: FREE ON (BEFORE CALLOC): %p\n", image->dst.data);
#endif
		util_memory_free(image->dst.data);
		image->dst.data = NULL;
	}
	image->dst.data = util_memory_calloc(sizeof(uint8_t),
			image->dst.rect.width * image->dst.rect.height * 4);
#ifdef DEBUG
	printf("image->dst.data: CALLOC ON: %p\n", image->dst.data);
#endif

	stbir_resize_uint8(image->src.data, image->src.width, image->src.height, 0,
			image->dst.data, image->dst.rect.width, image->dst.rect.height, 0,
			4);

	// ARGB to ABGR
	uint32_t *dp = (uint32_t *) image->dst.data;
	for(int i = 0, len = image->dst.rect.width * image->dst.rect.height;
			i < len;
			++i)
	{
		const uint8_t alpha = (dp[i] >> 24) & 0xFF;
		if (alpha < 255)
		{
			dp[i] = ui_xcb_Image__colorsBlendAlpha(dp[i],
					backgroundColor,
					256 - alpha);
		}

		dp[i] = (dp[i] & 0xff00ff00) | ((dp[i] >> 16) & 0xFF) | ((dp[i] << 16) & 0xFF0000);
	}
}

void
ui_xcb_Image_createPixmap(struct ui_xcb_Image *image)
{
	image->gc = xcb_generate_id(image->context->connection);
	image->pixmap = xcb_generate_id(image->context->connection);

	xcb_create_pixmap(image->context->connection,
			image->context->depth,
			image->pixmap,
			image->drawable,
			image->dst.rect.width,
			image->dst.rect.height);

	xcb_create_gc(image->context->connection,
			image->gc,
			image->pixmap,
			XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES,
			(uint32_t [2]) {
				0xFFFFFF,
				0
			});

	xcb_image_t *xcbImage = xcb_image_create_native(image->context->connection,
			image->dst.rect.width,
			image->dst.rect.height,
			XCB_IMAGE_FORMAT_Z_PIXMAP,
			image->context->depth,
			image->dst.data,
			image->dst.rect.width * image->dst.rect.height * 4,
			image->dst.data);
	util_memory_markAsFree(image->dst.data);
	image->dst.data = NULL;

	xcb_image_put(image->context->connection,
			image->pixmap,
			image->gc,
			xcbImage,
			0, 0, 0);

	xcb_image_destroy(xcbImage);
}

void
ui_xcb_Image_createResizedImagePixmap(struct ui_xcb_Image *image,
		const xcb_rectangle_t rect,
		const enum ui_xcb_Image_KeepAspect keepAspect)
{
	if (image->hasPixmapGc)
	{
		ui_xcb_Image__destroyPixmapGc(image);
	}
	ui_xcb_Image_resizeImage(image, rect, keepAspect);
	ui_xcb_Image_createPixmap(image);
}

void
ui_xcb_Image_render(struct ui_xcb_Image *image)
{
	xcb_copy_area(image->context->connection,
			image->pixmap,
			image->drawable,
			image->gc,
			0, 0,
			image->dst.rect.x,
			image->dst.rect.y,
			image->dst.rect.width,
			image->dst.rect.height);
}

