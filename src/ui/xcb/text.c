#include "ui/xcb/text.h"

const char *UI_XCB_TEXT_DEFAULT_FT_FONT = "Monospace 12";

struct ui_xcb_ScaleColor
{
	double red;
	double green;
	double blue;
	double alpha;
};

static struct ui_xcb_ScaleColor
ui_xcb_Text__u32ToScaleColorA(const uint32_t src, const double alpha)
{
	return (struct ui_xcb_ScaleColor) {
		.red = (double) ((src >> 16) & 0xFF) / 256.0,
		.green = (double) ((src >> 4) & 0xFF) / 256.0,
		.blue = (double) (src & 0xFF) / 256.0,
		.alpha = alpha,
	};
}

void
ui_xcb_Text_init(struct ui_xcb_Text *text,
		struct ui_xcb_Context *context,
		const char *fontConfig)
{
	text->cr_surface = cairo_xcb_surface_create(context->connection,
			context->screen->root,
			context->visual_type,
			context->screen->width_in_pixels,
			context->screen->height_in_pixels);
	text->cr = cairo_create(text->cr_surface);
	cairo_paint(text->cr);

	text->pa_layout = pango_cairo_create_layout(text->cr);
	PangoFontDescription *pa_desc = pango_font_description_from_string(fontConfig);
	pango_layout_set_font_description(text->pa_layout, pa_desc);
	pango_font_description_free(pa_desc);
	pa_desc = NULL;

	// Calculate symbols lengths
	
	char tmpStr[4] = { '\0' };
	tmpStr[0] = 'A';
	tmpStr[1] = 'A';
	int32_t width = 0;
	// Set base width
	pango_layout_set_text(text->pa_layout, tmpStr, -1);
	//cairo_move_to(text->cr, 0, 0);
	//pango_cairo_update_layout(text->cr, text->pa_layout);
	pango_layout_get_pixel_size(text->pa_layout, &width, NULL);
	const uint32_t baseWidth = width;
	text->mostSymbolsWidth = 0;

	for (uint32_t c = ' '; c < 127; ++c)
	{
		tmpStr[2] = c;
		pango_layout_set_text(text->pa_layout, tmpStr, -1);
		//cairo_move_to(text->cr, 0, 0);
		//pango_cairo_update_layout(text->cr, text->pa_layout);
		pango_layout_get_pixel_size(text->pa_layout, &width, NULL);

		text->symbolsWidth[c] = width - baseWidth;
		//printf("%c width: %d\n", c, text->symbolsWidth[c]);

		if (text->symbolsWidth[c] > text->mostSymbolsWidth)
		{
			text->mostSymbolsWidth = text->symbolsWidth[c];
		}
	}

	text->context = context;
}

void
ui_xcb_Text_deinit(struct ui_xcb_Text *text)
{
	g_object_unref(text->pa_layout);
	cairo_destroy(text->cr);
	cairo_surface_finish(text->cr_surface);
	cairo_surface_destroy(text->cr_surface);
}

void
ui_xcb_Text_GDEINIT(void)
{
	pango_cairo_font_map_set_default(NULL);
	//cairo_debug_reset_static_data();
	FcFini();
}

void
ui_xcb_Text_render(struct ui_xcb_Text *text,
		const xcb_drawable_t drawable,
		const char *str,
		const double x,
		const double y,
		const uint32_t color,
		const double alpha)
{
	ui_xcb_Text_renderWrapped(text, drawable, str, x, y, color, alpha,
			false, -1, 0, true);
}

int32_t
ui_xcb_Text_renderWrapped(struct ui_xcb_Text *text,
		const xcb_drawable_t drawable,
		const char *str,
		const double x,
		const double y,
		const uint32_t color,
		const double alpha,
		const bool wrapped,
		const uint32_t maxWidth,
		const uint32_t spacing,
		const bool render)
{
	int32_t height = spacing;

	if (render)
	{
		xcb_get_geometry_cookie_t geom_cookie = xcb_get_geometry(
				text->context->connection,
				drawable);

		xcb_get_geometry_reply_t *geom_reply = xcb_get_geometry_reply(
				text->context->connection,
				geom_cookie,
				NULL);

		if (!geom_reply)
		{
			fprintf(stderr, "ERROR: Cannot get gemetry of drawable %d.\n",
					drawable);
			return spacing;
		}

		cairo_surface_flush(text->cr_surface);
		cairo_xcb_surface_set_drawable(text->cr_surface,
				drawable,
				geom_reply->width,
				geom_reply->height);

		//printf("geom_reply: %d %d\n", geom_reply->width, geom_reply->height);

		free(geom_reply);
		geom_reply = NULL;
	}

	struct ui_xcb_ScaleColor scCol = ui_xcb_Text__u32ToScaleColorA(color, alpha);
	//printf("scCol: %.2f %.2f %.2f %.2f\n", scCol.red, scCol.green, scCol.blue, scCol.alpha);
	
	pango_layout_set_text(text->pa_layout, str, -1);
	if (wrapped)
	{
		pango_layout_set_width(text->pa_layout, maxWidth * PANGO_SCALE);
		pango_layout_set_wrap(text->pa_layout, PANGO_WRAP_WORD);
	}
	else
	{
		pango_layout_set_width(text->pa_layout, -1);
	}

	if (render)
	{
		cairo_set_source_rgba(text->cr, scCol.red, scCol.green, scCol.blue, scCol.alpha);
		cairo_move_to(text->cr, x, y);
	}
	pango_cairo_update_layout(text->cr, text->pa_layout);
	pango_layout_get_pixel_size(text->pa_layout, NULL, &height);
	if (render)
	{
		pango_cairo_show_layout(text->cr, text->pa_layout);
		cairo_surface_flush(text->cr_surface);
	}

	return height;
}

inline int32_t
ui_xcb_Text_fakeRenderWrapped(struct ui_xcb_Text *text,
		const char *str,
		const double y,
		const uint32_t maxWidth,
		const uint32_t spacing)
{
	return ui_xcb_Text_renderWrapped(text, 0, str, 0, y, 0, 0,
			true, maxWidth, spacing, false);
}

uint32_t
ui_xcb_Text_calcWidth(struct ui_xcb_Text *text,
		const char *str)
{
	const uint32_t strL = strlen(str);
	uint32_t totalWidth = 0;

	for (uint32_t i = 0; i < strL; ++i)
	{
		totalWidth += text->symbolsWidth[(uint32_t) str[i]];
	}

	return totalWidth;
}

