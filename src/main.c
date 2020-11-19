#include <stdio.h>

#include "util/memory.h"
#include "util/socket.h"
#include "util/stack.h"

#include "protocol/client.h"
#include "protocol/parser.h"
#include "protocol/xcb.h"
#include "protocol/historyStack.h"

#include "protocol/gemini/header.h"

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

#include "gemcx/xcb/controlBar.h"
#include "gemcx/xcb/connectUrl.h"
#include "gemcx/xcb/globals.h"
#include "gemcx/xcb/eventResponse.h"

int
main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

#ifdef DEBUG
	util_memory_enableDebug();
	printf("NOTICE: RUNNING DEBUG BUILD\n");
#endif
	//const char *startUrl = "gopher://gopher.quux.org/1/";
	//const char *startUrl = "file://example/out.gopher";
	const char *startUrl = "gemini://gemini.circumlunar.space/";
	//const char *startUrl = "file://example/test.gmi";
	//const char *startUrl = "file://example/out.gmi";
	
	gemcx_xcb_Globals_init(startUrl);
	xcb_flush(globals.context.connection);
	while (ui_xcb_Event_waitForEvent(&globals.event))
	{
		gemcx_xcb_EventResponse_response(globals.event.response_type & ~0x80,
				globals.event.generic_event);
	}

	gemcx_xcb_Globals_deinit();
	util_memory_freeAll();
	return 0;
}

