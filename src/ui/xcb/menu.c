#include "ui/xcb/menu.h"

#include "util/memory.h"

enum
{
	MENU_BUT_ALLOC = 8
};

void
ui_xcb_Menu_init(struct ui_xcb_Menu *const restrict menu,
		struct ui_xcb_Context *const restrict context,
		struct ui_xcb_Text *const restrict font,
		const xcb_window_t parentWindow,
		const struct ui_xcb_MenuStyle style)
{
	menu->buttonsAlloc = MENU_BUT_ALLOC;
	menu->buttons = util_memory_calloc(
			sizeof(struct ui_xcb_Button), menu->buttonsAlloc);
	menu->buttonsLength = 0;

	ui_xcb_Subwindow_init(&menu->subwindow,
			context,
			parentWindow,
			style.backgroundColor,
			0x000000,
			1,
			style.subWindowRect,
			XCB_EVENT_MASK_BUTTON_RELEASE |
			XCB_EVENT_MASK_BUTTON_PRESS |
			XCB_EVENT_MASK_BUTTON_RELEASE);

	menu->style = style;
	menu->buttonY = 0;
	menu->visible = false;

	menu->context = context;
	menu->font = font;
}

void
ui_xcb_Menu_deinit(struct ui_xcb_Menu *const restrict menu)
{
	if (menu->buttons != NULL)
	{
		for (uint32_t i = 0; i < menu->buttonsLength; ++i)
		{
			ui_xcb_Button_deinit(&menu->buttons[i]);
		}
		util_memory_free(menu->buttons);
	}
	menu->buttonsAlloc = 0;
	menu->buttonsLength = 0;

	ui_xcb_Subwindow_deinit(&menu->subwindow);

	menu->visible = false;
	menu->buttonY = 0;
	menu->context = NULL;
	menu->font = NULL;
}

static void
ui_xcb_Menu__expand(struct ui_xcb_Menu *const restrict menu)
{
	menu->buttonsAlloc += MENU_BUT_ALLOC;
	menu->buttons = util_memory_realloc(menu->buttons,
			sizeof(struct ui_xcb_Button) * menu->buttonsAlloc);
}

void
ui_xcb_Menu_add(struct ui_xcb_Menu *const restrict menu,
		const char *const restrict label)
{
	if (menu->buttonsLength >= (menu->buttonsAlloc - 1))
	{
		ui_xcb_Menu__expand(menu);
	}

	struct ui_xcb_Button *const newButton = &menu->buttons[menu->buttonsLength++];
	ui_xcb_Button_init(newButton, label, menu->font, menu->context,
			menu->subwindow.id,
			menu->style.backgroundColor,
			menu->style.textColor,
			0, 0,
			(const xcb_rectangle_t) {
				.x = 0,
				.y = menu->buttonY,
				.width = menu->style.subWindowRect.width,
				.height = menu->style.buttonHeight
			},
			menu->style.textX,
			menu->style.textY);

	menu->buttonY += menu->style.buttonHeight;

	// Update subwindow height to match total button heights
	menu->subwindow.rect.height = menu->buttonY;
	ui_xcb_Subwindow_applyAttributes(&menu->subwindow);
}

void
ui_xcb_Menu_show(struct ui_xcb_Menu *const restrict menu,
		const bool show)
{
	menu->visible = show;
	ui_xcb_Subwindow_show(&menu->subwindow, show);
}

void
ui_xcb_Menu_render(struct ui_xcb_Menu *const restrict menu)
{
	if (menu->visible)
	{
		for (uint32_t i = 0; i < menu->buttonsLength; ++i)
		{
			ui_xcb_Button_render(&menu->buttons[i]);
		}
	}
}

int32_t
ui_xcb_Menu_pressed(struct ui_xcb_Menu *const restrict menu,
		const xcb_button_press_event_t *const restrict bpEv)
{
#if 0
	printf("event: %d | root: %d | child: %d\n",
			bpEv->event, bpEv->root, bpEv->child);
#endif
	if (bpEv->event == menu->subwindow.id)
	{
		return -2;
	}

	for (uint32_t i = 0; i < menu->buttonsLength; ++i)
	{
		if (bpEv->event == menu->buttons[i].subwindow.id)
		{
			return i;
		}
	}

	return -1;
}

