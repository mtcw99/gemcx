#include "gemcx/xcb/controlBar.h"

static const struct
{
	const char *label;
} gemcxMenuList[GEMCX_XCB_CONTROLBAR_MENU__TOTAL] = {
	[GEMCX_XCB_CONTROLBAR_MENU_SETTINGS] = {"Settings"},
	[GEMCX_XCB_CONTROLBAR_MENU_EXIT] = {"Exit"},
};

void
gemcx_xcb_ControlBar_init(struct gemcx_xcb_ControlBar *controlBar,
		struct ui_xcb_Context *context,
		const xcb_window_t windowId,
		const uint32_t barHeight,
		char *urlStr,
		const uint32_t urlStrLen,
		struct ui_xcb_Text *fonts)
{
	// ControlBar Subwindow
	ui_xcb_Subwindow_init(&controlBar->subwindow, context, windowId,
			0x111111, 0xEEEEEE, 0,
			(const xcb_rectangle_t) { 0, 0, 640, barHeight },
			XCB_EVENT_MASK_EXPOSURE 	|
			XCB_EVENT_MASK_KEY_PRESS 	|
			XCB_EVENT_MASK_KEY_RELEASE 	|
			XCB_EVENT_MASK_BUTTON_PRESS 	|
			XCB_EVENT_MASK_BUTTON_RELEASE);
	ui_xcb_Subwindow_show(&controlBar->subwindow, true);

	// Main Dropdown Menu button
	ui_xcb_Button_init(&controlBar->buttonMenu, "gemcx", &fonts[0],
			context, controlBar->subwindow.id,
			0x444444, 0xEEEEEE, 0x000000, 0,
			(const xcb_rectangle_t) { 0, 0, 70, barHeight },
			10, 2,
			0x222222, 0xFFFFFF);
	uint32_t controlBarX = 70;

	// Back button
	ui_xcb_Button_init(&controlBar->buttonBack, "<=", &fonts[4],
			context, controlBar->subwindow.id,
			0x444444, 0xEEEEEE, 0x000000, 0,
			(const xcb_rectangle_t) { controlBarX, 0, 50, barHeight },
			10, 5,
			0x222222, 0xFFFFFF);
	controlBarX += 50;

	// Forward button
	ui_xcb_Button_init(&controlBar->buttonForward, "=>", &fonts[4],
			context, controlBar->subwindow.id,
			0x444444, 0xEEEEEE, 0x000000, 0,
			(const xcb_rectangle_t) { controlBarX, 0, 50, barHeight },
			10, 5,
			0x222222, 0xFFFFFF);
	controlBarX += 50;

	// Main Dropdown Menu
	ui_xcb_Menu_init(&controlBar->menu, context, &fonts[0],
			windowId,
			(const struct ui_xcb_MenuStyle) {
				.backgroundColor = 0x444444,
				.textColor = 0xEEEEEE,
				.textX = 15,
				.textY = 4,
				.subWindowRect = { 0, barHeight, 150, 400 },
				.buttonHeight = barHeight,
				.hoverBackgroundColor = 0x333333,
				.hoverTextColor = 0xFFFFFF
			});
	for (uint32_t i = 0; i < GEMCX_XCB_CONTROLBAR_MENU__TOTAL; ++i)
	{
		ui_xcb_Menu_add(&controlBar->menu, gemcxMenuList[i].label);
	}

	// URL Input Bar
	ui_xcb_TextInput_init(&controlBar->urlInput, context, &fonts[4],
			controlBar->subwindow.id,
			(const xcb_rectangle_t) {
				.x = controlBarX,
				.y = 0,
				.width = 400,
				.height = barHeight
			},
			0x000000, 0xDDDDDD,
			10, 5,
			urlStr, urlStrLen);

	controlBar->urlInputXOffset = controlBarX;
}

void
gemcx_xcb_ControlBar_deinit(struct gemcx_xcb_ControlBar *controlBar)
{
	controlBar->urlInputXOffset = 0;
	ui_xcb_Menu_deinit(&controlBar->menu);
	ui_xcb_TextInput_deinit(&controlBar->urlInput);
	ui_xcb_Button_deinit(&controlBar->buttonForward);
	ui_xcb_Button_deinit(&controlBar->buttonBack);
	ui_xcb_Button_deinit(&controlBar->buttonMenu);
	ui_xcb_Subwindow_deinit(&controlBar->subwindow);
}

void
gemcx_xcb_ControlBar_configureNotify(struct gemcx_xcb_ControlBar *controlBar,
		const xcb_configure_notify_event_t *cnEvent)
{
	controlBar->subwindow.rect.width = cnEvent->width;
	controlBar->urlInput.subwindow.rect.width = cnEvent->width -
			controlBar->urlInputXOffset;

	ui_xcb_Subwindow_applyAttributes(&controlBar->subwindow);
	ui_xcb_Subwindow_applyAttributes(&controlBar->urlInput.subwindow);
}

void
gemcx_xcb_ControlBar_expose(struct gemcx_xcb_ControlBar *controlBar,
		const xcb_expose_event_t *exEv)
{
	(void) exEv;
	ui_xcb_Button_render(&controlBar->buttonMenu);
	ui_xcb_Button_render(&controlBar->buttonBack);
	ui_xcb_Button_render(&controlBar->buttonForward);
	ui_xcb_TextInput_render(&controlBar->urlInput);
	ui_xcb_Menu_render(&controlBar->menu);
}

bool
gemcx_xcb_ControlBar_enterNotify(struct gemcx_xcb_ControlBar *controlBar,
		const xcb_enter_notify_event_t *enterEv)
{
	if (ui_xcb_Button_hoverEnter(&controlBar->buttonMenu, enterEv))
	{
		return true;
	}
	else if (ui_xcb_Button_hoverEnter(&controlBar->buttonBack, enterEv))
	{
		return true;
	}
	else if (ui_xcb_Button_hoverEnter(&controlBar->buttonForward, enterEv))
	{
		return true;
	}
	else if (ui_xcb_Menu_hoverEnter(&controlBar->menu, enterEv))
	{
		return true;
	}

	return false;
}

bool
gemcx_xcb_ControlBar_leaveNotify(struct gemcx_xcb_ControlBar *controlBar,
		const xcb_leave_notify_event_t *leaveEv)
{
	if (ui_xcb_Button_hoverLeave(&controlBar->buttonMenu, leaveEv))
	{
		return true;
	}
	else if (ui_xcb_Button_hoverLeave(&controlBar->buttonBack, leaveEv))
	{
		return true;
	}
	else if (ui_xcb_Button_hoverLeave(&controlBar->buttonForward, leaveEv))
	{
		return true;
	}
	else if (ui_xcb_Menu_hoverLeave(&controlBar->menu, leaveEv))
	{
		return true;
	}

	return false;
}

bool
gemcx_xcb_ControlBar_buttonPress(struct gemcx_xcb_ControlBar *controlBar,
		const xcb_button_press_event_t *bpEv,
		struct ui_xcb_Event *event,
		struct protocol_HistoryStack *historyStack,
		bool *connUrlRR,
		char *urlStr)
{
	switch (bpEv->detail)
	{
	case 1:	// Left-mouse click
		controlBar->urlInput.active = (controlBar->urlInput.subwindow.id == bpEv->event);

		int32_t menuRetVal = -1;
		bool closeMenuAftEv = true;
		if ((menuRetVal = ui_xcb_Menu_pressed(&controlBar->menu, bpEv)) >= 0)
		{
			switch (menuRetVal)
			{
			case GEMCX_XCB_CONTROLBAR_MENU_SETTINGS:
				printf("settings\n");
				break;
			case GEMCX_XCB_CONTROLBAR_MENU_EXIT:
				ui_xcb_Event_close(event);
				break;
			}
		}

		if (controlBar->menu.visible && closeMenuAftEv)
		{
			ui_xcb_Menu_show(&controlBar->menu, false);
		}

		if (menuRetVal >= 0)
		{
			return true;
		}

		if (ui_xcb_Button_pressed(&controlBar->buttonMenu, bpEv->event))
		{
			ui_xcb_Menu_show(&controlBar->menu, true);
			return true;
		}
		else if (ui_xcb_Button_pressed(&controlBar->buttonBack, bpEv->event))
		{
			// Back button action
			if (protocol_HistoryStack_enableBackward(historyStack))
			{
				printf("Back\n");
				protocol_HistoryStack_pop(historyStack);
				char *popUrl = protocol_HistoryStack_pop(historyStack);
				printf("URL: %s\n", popUrl);
				strcpy(urlStr, popUrl);
				*connUrlRR = true;
			}
			return true;
		}
		else if (ui_xcb_Button_pressed(&controlBar->buttonForward, bpEv->event))
		{
			// Forward button action
			if (protocol_HistoryStack_enableForward(historyStack))
			{
				printf("Forward\n");
				strcpy(urlStr, protocol_HistoryStack_forward(historyStack));
			}
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

void
gemcx_xcb_ControlBar_selectionNotify(struct gemcx_xcb_ControlBar *controlBar,
		struct ui_xcb_Clipboard *clipboard)
{
	if (clipboard->contentLength > 0)
	{
		if (controlBar->urlInput.active)
		{
			// Remove newline
			for (uint32_t i = 0; i < clipboard->contentLength; ++i)
			{
				if (clipboard->content[i] == '\n' ||
						clipboard->content[i] == '\r')
				{
					clipboard->content[i] = ' ';
				}
			}

			ui_xcb_TextInput_append(&controlBar->urlInput,
					clipboard->content,
					clipboard->contentLength);
			//printf("content: %s\n", clipboard->content);
		}
	}
}

