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
	cairo_move_to(text->cr, 0, 0);
	pango_cairo_update_layout(text->cr, text->pa_layout);
	pango_layout_get_pixel_size(text->pa_layout, &width, NULL);
	const uint32_t baseWidth = width;
	text->mostSymbolsWidth = 0;

	for (uint32_t c = ' '; c < 127; ++c)
	{
		tmpStr[2] = c;
		pango_layout_set_text(text->pa_layout, tmpStr, -1);
		cairo_move_to(text->cr, 0, 0);
		pango_cairo_update_layout(text->cr, text->pa_layout);
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
	cairo_debug_reset_static_data();
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
		return;
	}

	cairo_surface_flush(text->cr_surface);
	cairo_xcb_surface_set_drawable(text->cr_surface,
			drawable,
			geom_reply->width,
			geom_reply->height);

	//printf("geom_reply: %d %d\n", geom_reply->width, geom_reply->height);

	free(geom_reply);
	geom_reply = NULL;

	struct ui_xcb_ScaleColor scCol = ui_xcb_Text__u32ToScaleColorA(color, alpha);
	//printf("scCol: %.2f %.2f %.2f %.2f\n", scCol.red, scCol.green, scCol.blue, scCol.alpha);
	pango_layout_set_text(text->pa_layout, str, -1);
	cairo_set_source_rgba(text->cr, scCol.red, scCol.green, scCol.blue, scCol.alpha);
	cairo_move_to(text->cr, x, y);
	pango_cairo_update_layout(text->cr, text->pa_layout);
	//pango_layout_get_pixel_size(text->pa_layout, &text->width, &text->height);
	pango_cairo_show_layout(text->cr, text->pa_layout);

	cairo_surface_flush(text->cr_surface);
}

// TODO FIX: Wrapping off-screen-width bug
double
ui_xcb_Text_renderWrapped(struct ui_xcb_Text *text,
		const xcb_drawable_t drawable,
		const char *str,
		const double x,
		const double y,
		const uint32_t color,
		const double alpha,
		const uint32_t maxWidth,
		const uint32_t spacing)
{
	const uint32_t strSize = strlen(str);
	uint32_t width = 0;

	char *tmpStr = calloc(sizeof(char), strSize + 1);
	strcpy(tmpStr, str);
	uint32_t splitIdx[128] = { 0 };
	uint32_t splitIdxSize = 0;
	uint32_t nextMaxWidth = maxWidth;
	double wrY = y;

	for (uint32_t i = 0, prevWS = 0; i < strSize; ++i)
	{
		if ((uint32_t) str[i] < 127)
		{
			width += text->symbolsWidth[(uint32_t) str[i]];
		}
		else
		{
			width += text->mostSymbolsWidth;
		}

		if (str[i] != ' ' && str[i] != '\n')
		{
			continue;
		}

		const uint32_t whspIdx = i;
		tmpStr[whspIdx] = '\0';

		if ((width >= nextMaxWidth) && (i > 0))
		{
#if 0
			printf("(%d/%d) [%d] tmpStr: %s\n", width, nextMaxWidth, i -1, tmpStr);
#endif
			const uint32_t splIndex = splitIdxSize;
			splitIdx[splitIdxSize++] = prevWS;
			nextMaxWidth += maxWidth;

			const uint32_t strIdxL = (splIndex == 0) ? 0 : (splitIdx[splIndex - 1] + 1);
			const uint32_t strIdxR = splitIdx[splIndex];
			const uint32_t splLen = strIdxR - strIdxL;

			//printf("SPLIT at: %d - %d (%d)\n", strIdxL, strIdxR, splLen);
			//strncpy(tmpStr, str + strIdxL, splLen);
			tmpStr[splLen + strIdxL] = '\0';
			ui_xcb_Text_render(text, drawable,
					tmpStr + strIdxL, x, wrY, color, alpha);
			wrY += spacing; // spacing
			tmpStr[splLen + strIdxL] = ' ';
		}
		tmpStr[whspIdx] = ' ';
		prevWS = whspIdx;
	}

	const uint32_t splIndex = splitIdxSize;
	splitIdx[splitIdxSize++] = strSize;
	const uint32_t strIdxL = (splIndex == 0) ? 0 : (splitIdx[splIndex - 1] + 1);
	const uint32_t strIdxR = splitIdx[splIndex];
	const uint32_t splLen = strIdxR - strIdxL;

	//printf("SPLIT at: %d - %d (%d)\n", strIdxL, strIdxR, splLen);
	//strncpy(tmpStr, str + strIdxL, splLen);
	tmpStr[splLen + strIdxL] = '\0';
	ui_xcb_Text_render(text, drawable,
			tmpStr + strIdxL, x, wrY, color, alpha);
	wrY += spacing; // spacing

	free(tmpStr);

	return wrY - y;
}

