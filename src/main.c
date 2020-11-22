#include <stdio.h>

#include "util/memory.h"

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
	const char *startUrl =
		"gemini://gemini.circumlunar.space/";
		//"gopher://gopher.quux.org/1/";
		//"file://example/out.gopher";
		//"file://example/test.gmi";
		//"file://example/out.gmi";
	
	gemcx_xcb_Globals_init(startUrl);
	xcb_flush(globals.context.connection);
	while (ui_xcb_Event_waitForEvent(&globals.event))
	{
		gemcx_xcb_EventResponse_response(
				globals.event.response_type & ~0x80,
				globals.event.generic_event);
	}

	gemcx_xcb_Globals_deinit();
	util_memory_freeAll();
	return 0;
}

