#include <stdio.h>

#include "util/memory.h"

#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#pragma GCC diagnostic pop

#include "gemcx/xcb/controlBar.h"
#include "gemcx/xcb/connectUrl.h"
#include "gemcx/xcb/globals.h"
#include "gemcx/xcb/eventResponse.h"
#include "gemcx/config.h"

int
main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

#ifdef DEBUG
	util_memory_enableDebug();
	printf("NOTICE: RUNNING DEBUG BUILD\n");
#endif
	
	gemcx_Config_initDefaults();
	gemcx_xcb_Globals_init();

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

