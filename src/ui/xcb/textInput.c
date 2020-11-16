#include "ui/xcb/textInput.h"

#include <string.h>
#include "util/memory.h"

void
ui_xcb_TextInput_init(struct ui_xcb_TextInput * const textinput,
		struct ui_xcb_Context * const context,
		struct ui_xcb_Text * const font,
		const xcb_window_t parentWindow,
		const xcb_rectangle_t rect,
		const uint32_t backgroundColor,
		const uint32_t textColor,
		const uint32_t textX,
		const uint32_t textY,
		char * const str,
		const uint32_t strMax)
{
	ui_xcb_Subwindow_init(&textinput->subwindow,
			context, parentWindow,
			backgroundColor, 0, 0, rect,
			XCB_EVENT_MASK_EXPOSURE 	|
			XCB_EVENT_MASK_KEY_PRESS 	|
			XCB_EVENT_MASK_KEY_RELEASE 	|
			XCB_EVENT_MASK_BUTTON_PRESS 	|
			XCB_EVENT_MASK_BUTTON_RELEASE	);
	ui_xcb_Subwindow_show(&textinput->subwindow, true);

	ui_xcb_Pixmap_init(&textinput->pixmap,
			context, textinput->subwindow.id,
			(strMax + 1) * font->mostSymbolsWidth,
			rect.height,
			backgroundColor);

	textinput->textColor = textColor;
	textinput->active = false;
	textinput->textX = textX;
	textinput->textY = textY;

	textinput->str = str;
	textinput->strMax = strMax;
	textinput->strLen = strlen(str);

	textinput->font = font;
	textinput->context = context;
}

void
ui_xcb_TextInput_deinit(struct ui_xcb_TextInput * const textinput)
{
	ui_xcb_Pixmap_deinit(&textinput->pixmap);
	ui_xcb_Subwindow_deinit(&textinput->subwindow);

	textinput->active = false;

	textinput->str = NULL;
	textinput->strMax = 0;
	textinput->strLen = 0;

	textinput->font = NULL;
	textinput->context = NULL;
}

void
ui_xcb_TextInput_render(struct ui_xcb_TextInput * const textinput)
{
	ui_xcb_Pixmap_render(&textinput->pixmap,
			(textinput->renderWidth > textinput->subwindow.rect.width) ?
				textinput->subwindow.rect.width - textinput->renderWidth :
				0,
			0);
}

void
ui_xcb_TextInput_modify(struct ui_xcb_TextInput * const textinput,
		const struct ui_xcb_Key * const xkey)
{
	if (!textinput->active)
	{
		return;
	}

	const uint32_t bufferLen = strlen(xkey->buffer);
	bool reRender = false;

	switch (xkey->keysymNoMask)
	{
	case XKB_KEY_BackSpace:
		if (textinput->strLen > 0)
		{
			textinput->str[--textinput->strLen] = '\0';
			reRender = true;
		}
		break;
	default:
		if ((textinput->strLen + bufferLen) < textinput->strMax)
		{
			strcat(textinput->str, xkey->buffer);
			textinput->strLen += bufferLen;
			reRender = true;
		}
		break;
	}

	if (reRender)
	{
		ui_xcb_Pixmap_clear(&textinput->pixmap);
		textinput->renderWidth = ui_xcb_Text_render(textinput->font,
				textinput->pixmap.pixmap,
				textinput->str,
				textinput->textX,
				textinput->textY,
				textinput->textColor,
				1.0).width;
		ui_xcb_TextInput_render(textinput);
	}
}

