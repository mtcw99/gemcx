#ifndef GEMCX_XCB_GLOBALS_H
#define GEMCX_XCB_GLOBALS_H

#include "protocol/client.h"
#include "protocol/historyStack.h"
#include "protocol/gemini/header.h"

#include "parser/generic.h"
#include "render/xcb.h"

#include "ui/xcb/context.h"
#include "ui/xcb/key.h"
#include "ui/xcb/window.h"
#include "ui/xcb/event.h"
#include "ui/xcb/text.h"
#include "ui/xcb/pixmap.h"
#include "ui/xcb/subwindow.h"
#include "ui/xcb/button.h"
#include "ui/xcb/cursor.h"
#include "ui/xcb/subwindow.h"
#include "ui/xcb/textInput.h"
#include "ui/xcb/clipboard.h"
#include "ui/xcb/menu.h"
#include "ui/xcb/image.h"

#include "gemcx/xcb/controlBar.h"
#include "gemcx/xcb/connectUrl.h"

struct gemcx_xcb_Globals
{
	struct protocol_Client client;
	struct Parser parser;
	struct ui_xcb_Context context;
	struct ui_xcb_Clipboard clipboard;
	struct ui_xcb_Window window;
	struct ui_xcb_Event event;
	struct ui_xcb_Text text[UI_XCB_TEXTTYPE__TOTAL];
	int32_t contentSubWindowYDiff;
	struct ui_xcb_Subwindow contentSubWindow;
	char urlStr[1024];
	struct gemcx_xcb_ControlBar controlBar;
	struct ui_xcb_Pixmap doubleBuffer;
	struct ui_xcb_Pixmap mainArea;
	int32_t mainAreaYoffset;
	uint32_t mainAreaYMax;
	struct ui_xcb_Key xkey;
	struct render_Xcb pxcb;
	struct protocol_HistoryStack historyStack;
	struct gemcx_xcb_ConnectUrl connectUrl;

	struct ui_xcb_Image sampleImage;
};

extern struct gemcx_xcb_Globals globals;

void gemcx_xcb_Globals_init(void);
void gemcx_xcb_Globals_deinit(void);

#endif // GEMCX_XCB_GLOBALS_H

