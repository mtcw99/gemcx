#include <stdio.h>

// gcc -o gemcx -Iinclude -std=c99 -pedantic -Wall -Wextra -Os src/main.c src/gemini/parser.c

#include "gemini/parser.h"

int
main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	struct gemini_Parser parser = { 0 };
	gemini_Parser_init(&parser);
	gemini_Parser_parse(&parser, "example/test.gmi");
	//gemini_Parser_print(&parser);
	gemini_Parser_render(&parser);
	gemini_Parser_deinit(&parser);

	return 0;
}

