#include <stdio.h>

#include "gemini/parser.h"
#include "gemini/client.h"
#include "gemini/header.h"
#include "ui/xcb/context.h"
#include "ui/xcb/key.h"
#include "ui/xcb/window.h"
#include "util/memory.h"

int
main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	//util_memory_enableDebug();

#if 0
	if (argc < 2)
	{
		return 1;
	}
	gemini_Client_GINIT();

	FILE *tmpf = tmpfile();

	struct gemini_Client client = { 0 };
	gemini_Client_init(&client, argv[1]);
	gemini_Client_printInfo(&client);
	enum gemini_Client_ConnectError connErr = gemini_Client_request(&client, tmpf);
	if (connErr != GEMINI_CLIENT_CONNECTERROR_NONE)
	{
		fprintf(stderr, "ERROR %d has occured: %s\n",
				connErr, gemini_Client_getErrorStr(connErr));
		return 1;
	}

	struct gemini_Header header = { 0 };
	gemini_Header_get(&header, tmpf);
	gemini_Header_print(&header);
#endif

	struct gemini_Parser parser = { 0 };
	gemini_Parser_init(&parser);
	//gemini_Parser_parseFp(&parser, tmpf);
	gemini_Parser_parse(&parser, "example/out.gmi");
	//gemini_Parser_print(&parser);
	//gemini_Parser_render(&parser);

#if 0
	FILE *givenFile = fopen("example/out.gmi", "w");
	rewind(tmpf);
	if (fgets(header, 1027, tmpf) != NULL)
	{
		printf("header: %s\n", header);
	}

	while (fgets(header, 1027, tmpf) != NULL)
	{
		fprintf(givenFile, "%s", header);
	}

	fclose(givenFile);
#endif

#if 0
	gemini_Header_print(&header);
	if (gemini_Header_isGemini(&header))
	{
		printf("Gemini header\n");
	}

	fclose(tmpf);
#endif

	// XCB TEMPS
	
#if 0
	struct ui_xcb_Context context = { 0 };
	ui_xcb_Context_init(&context);

	struct ui_xcb_Window window = { 0 };
	ui_xcb_Window_init(&window, &context, "gemcx");

	ui_xcb_Window_deinit(&window);
	ui_xcb_Context_deinit(&context);
#endif

	util_memory_freeAll();
	return 0;
}

