#ifndef UI_XCB_TEXT_H
#define UI_XCB_TEXT_H

#include <stdint.h>
#include <stdbool.h>
#include <xcb/xcb.h>

#include <cairo/cairo-xcb.h>
#include <pango/pangocairo.h>
#include <fontconfig/fontconfig.h>

#include "ui/xcb/context.h"

extern const char *UI_XCB_TEXT_DEFAULT_FT_FONT;

struct ui_xcb_Text
{
	cairo_surface_t	*cr_surface;
	cairo_t		*cr;
	PangoLayout	*pa_layout;
	struct ui_xcb_Context	*context;
	uint32_t	symbolsWidth[128];
	uint32_t	mostSymbolsWidth;
};

enum ui_xcb_TextType
{
	UI_XCB_TEXTTYPE_TEXT,	// Gemini defaults to this
	UI_XCB_TEXTTYPE_H1,
	UI_XCB_TEXTTYPE_H2,
	UI_XCB_TEXTTYPE_H3,
	UI_XCB_TEXTTYPE_PRE,	// Gopher and text file defaults to this

	UI_XCB_TEXTTYPE__TOTAL
};

void ui_xcb_Text_init(struct ui_xcb_Text *text, struct ui_xcb_Context *context,
		const char *fontConfig);
void ui_xcb_Text_deinit(struct ui_xcb_Text *text);
void ui_xcb_Text_GDEINIT(void);

void ui_xcb_Text_render(struct ui_xcb_Text *text,
		const xcb_drawable_t drawable,
		const char *str,
		const double x,
		const double y,
		const uint32_t color,
		const double alpha);

double ui_xcb_Text_renderWrapped(struct ui_xcb_Text *text,
		const xcb_drawable_t drawable,
		const char *str,
		const double x,
		const double y,
		const uint32_t color,
		const double alpha,
		const uint32_t maxWidth,
		const uint32_t spacing,
		const bool render);

double ui_xcb_Text_fakeRenderWrapped(struct ui_xcb_Text *text,
		const char *str,
		const double y,
		const uint32_t maxWidth,
		const uint32_t spacing);

uint32_t ui_xcb_Text_calcWidth(struct ui_xcb_Text *text,
		const char *str);

#endif // UI_XCB_TEXT_H

