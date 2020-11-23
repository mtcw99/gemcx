#include "gemcx/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct gemcx_Config configGlobal = { 0 };

static const char *const defStrs[GEMCX_CONFIG_STR__TOTAL] = {
	[GEMCX_CONFIG_STR_STARTURL] = "gemini://gemini.circumlunar.space/",
	[GEMCX_CONFIG_STR_FONT_TEXT] = "noto sans normal 12",
	[GEMCX_CONFIG_STR_FONT_H1] = "noto sans bold 24",
	[GEMCX_CONFIG_STR_FONT_H2] = "noto sans bold 18",
	[GEMCX_CONFIG_STR_FONT_H3] = "noto sans bold 14",
	[GEMCX_CONFIG_STR_FONT_PRE] = "liberation mono 12",
};
static const int32_t defInts[GEMCX_CONFIG_INT__TOTAL] = {
	[GEMCX_CONFIG_INT_CONTROLBARHEIGHT] = 30,
};
static const bool defBools[GEMCX_CONFIG_BOOL__TOTAL] = {
	[GEMCX_CONFIG_BOOL_TEMP] = false,
};

void
gemcx_Config_initDefaults(void)
{
	for (uint32_t i = 0; i < GEMCX_CONFIG_STR__TOTAL; ++i)
	{
		strcpy(configGlobal.str[i], defStrs[i]);
	}

	for (uint32_t i = 0; i < GEMCX_CONFIG_INT__TOTAL; ++i)
	{
		configGlobal.ints[i] = defInts[i];
	}

	for (uint32_t i = 0; i < GEMCX_CONFIG_BOOL__TOTAL; ++i)
	{
		configGlobal.bools[i] = defBools[i];
	}
}

void
gemcx_Config_fromFile(const char *filePath)
{
	(void) filePath;
}

