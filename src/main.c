#include <stdio.h>

// gcc -o gemcx -Iinclude -lssl -lcrypto -std=c99 -pedantic -Wall -Wextra -Os src/main.c src/gemini/parser.c sr/gemini/client.c

#include "gemini/parser.h"
#include "gemini/client.h"

int
main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

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

	rewind(tmpf);
	char header[1027] = { 0 };
	if (fgets(header, 1027, tmpf) != NULL)
	{
		printf("header: %s\n", header);
	}

	struct gemini_Parser parser = { 0 };
	gemini_Parser_init(&parser);
	gemini_Parser_parseFp(&parser, tmpf);
	//gemini_Parser_parse(&parser, "example/out.gmi");
	//gemini_Parser_print(&parser);
	gemini_Parser_render(&parser);
	gemini_Parser_deinit(&parser);

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

	fclose(tmpf);
	return 0;
}

