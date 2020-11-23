#include "gemcx/xcb/globals.h"

#include "gemcx/config.h"

struct gemcx_xcb_Globals globals = { 0 };

void
gemcx_xcb_Globals_init(void)
{
	gemcx_xcb_ConnectUrl_connect(&globals.client, &globals.parser,
			configGlobal.str[GEMCX_CONFIG_STR_STARTURL]);
	ui_xcb_Context_init(&globals.context);
	ui_xcb_Clipboard_init(&globals.clipboard, &globals.context);

	ui_xcb_Window_init(&globals.window, &globals.context, "gemcx");
	ui_xcb_Window_minSize(&globals.window, 400, 200);
	ui_xcb_Window_map(&globals.window, true);

	ui_xcb_Event_init(&globals.event, globals.context.connection);

	const char *fontConfigs[UI_XCB_TEXTTYPE__TOTAL] = {
		[UI_XCB_TEXTTYPE_TEXT] = configGlobal.str[GEMCX_CONFIG_STR_FONT_TEXT],
		[UI_XCB_TEXTTYPE_H1] = configGlobal.str[GEMCX_CONFIG_STR_FONT_H1],
		[UI_XCB_TEXTTYPE_H2] = configGlobal.str[GEMCX_CONFIG_STR_FONT_H2],
		[UI_XCB_TEXTTYPE_H3] = configGlobal.str[GEMCX_CONFIG_STR_FONT_H3],
		[UI_XCB_TEXTTYPE_PRE] = configGlobal.str[GEMCX_CONFIG_STR_FONT_PRE]
	};
	for (uint32_t i = 0; i < UI_XCB_TEXTTYPE__TOTAL; ++i)
	{
		ui_xcb_Text_init(&globals.text[i], &globals.context, fontConfigs[i]);
	}

	globals.contentSubWindowYDiff = configGlobal.ints[GEMCX_CONFIG_INT_CONTROLBARHEIGHT];

	// Content sub-window
	ui_xcb_Subwindow_init(&globals.contentSubWindow, &globals.context, globals.window.id,
			0x222222, 0xEEEEEE, 0,
			(const xcb_rectangle_t) {
				.x = 0,
				.y = globals.contentSubWindowYDiff,
				.width = 640,
				.height = 480
			},
			XCB_EVENT_MASK_EXPOSURE 	|
			XCB_EVENT_MASK_KEY_PRESS 	|
			XCB_EVENT_MASK_KEY_RELEASE 	|
			XCB_EVENT_MASK_BUTTON_PRESS 	|
			XCB_EVENT_MASK_BUTTON_RELEASE);
	ui_xcb_Subwindow_show(&globals.contentSubWindow, true);

	strcpy(globals.urlStr, configGlobal.str[GEMCX_CONFIG_STR_STARTURL]);

	gemcx_xcb_ControlBar_init(&globals.controlBar,
			&globals.context,
			globals.window.id,
			globals.contentSubWindowYDiff,
			globals.urlStr, sizeof(globals.urlStr),
			globals.text);

	ui_xcb_Pixmap_init(&globals.doubleBuffer, &globals.context,
			globals.contentSubWindow.id,
			globals.context.rootWidth,
			globals.context.rootHeight, 0x000000);

	ui_xcb_Pixmap_init(&globals.mainArea, &globals.context,
			globals.doubleBuffer.pixmap,
			globals.context.rootWidth, 10080, 0x222222);
	globals.mainAreaYoffset = 0;
	globals.mainAreaYMax = 0;

	ui_xcb_Key_init(&globals.xkey, globals.context.connection);

	protocol_Xcb_init(&globals.pxcb, &globals.context,
			globals.text, globals.mainArea.pixmap,
			&globals.contentSubWindow,
			globals.context.rootWidth, 10080, 0x222222);

	protocol_Xcb_itemsInit(&globals.pxcb, &globals.parser);
	protocol_Xcb_padding(&globals.pxcb, 10, 10);

	protocol_HistoryStack_init(&globals.historyStack);
	protocol_HistoryStack_push(&globals.historyStack, globals.urlStr);

	globals.connectUrl.client = &globals.client;
	globals.connectUrl.parser = &globals.parser;
	globals.connectUrl.pxcb = &globals.pxcb;
	globals.connectUrl.mainAreaYoffset = &globals.mainAreaYoffset;
	globals.connectUrl.urlInput = &globals.controlBar.urlInput;
	globals.connectUrl.mainAreaYMax = &globals.mainAreaYMax;
	globals.connectUrl.mainArea = &globals.mainArea;
	globals.connectUrl.doubleBuffer = &globals.doubleBuffer;
	globals.connectUrl.historyStack = &globals.historyStack;
}

void
gemcx_xcb_Globals_deinit(void)
{
	protocol_HistoryStack_deinit(&globals.historyStack);

	protocol_Xcb_deinit(&globals.pxcb);
	gemcx_xcb_ControlBar_deinit(&globals.controlBar);

	// xcb deinit
	ui_xcb_Key_deinit(&globals.xkey);
	ui_xcb_Pixmap_deinit(&globals.doubleBuffer);
	ui_xcb_Pixmap_deinit(&globals.mainArea);
	for (uint32_t i = 0; i < UI_XCB_TEXTTYPE__TOTAL; ++i)
	{
		ui_xcb_Text_deinit(&globals.text[i]);
	}
	ui_xcb_Subwindow_deinit(&globals.contentSubWindow);
	ui_xcb_Event_deinit(&globals.event);
	ui_xcb_Window_deinit(&globals.window);
	ui_xcb_Clipboard_deinit(&globals.clipboard);
	ui_xcb_Context_deinit(&globals.context);

	ui_xcb_Text_GDEINIT();

	// protocols deinit
	protocol_Parser_deinit(&globals.parser);
}

