#ifndef GEMCX_CONFIG_H
#define GEMCX_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

enum gemcx_config_Str
{
	GEMCX_CONFIG_STR_STARTURL = 0,
	GEMCX_CONFIG_STR_FONT_TEXT,
	GEMCX_CONFIG_STR_FONT_H1,
	GEMCX_CONFIG_STR_FONT_H2,
	GEMCX_CONFIG_STR_FONT_H3,
	GEMCX_CONFIG_STR_FONT_PRE,
	GEMCX_CONFIG_STR_HTTP_OPEN,

	GEMCX_CONFIG_STR__TOTAL
};

enum gemcx_Config_Int
{
	GEMCX_CONFIG_INT_CONTROLBARHEIGHT = 0,

	GEMCX_CONFIG_INT__TOTAL
};

enum gemcx_Config_Bool
{
	GEMCX_CONFIG_BOOL_TEMP = 0,

	GEMCX_CONFIG_BOOL__TOTAL
};

struct gemcx_Config
{
	char str[GEMCX_CONFIG_STR__TOTAL][1024];
	int32_t ints[GEMCX_CONFIG_INT__TOTAL];
	bool bools[GEMCX_CONFIG_BOOL__TOTAL];
};

extern struct gemcx_Config configGlobal;

void gemcx_Config_initDefaults(void);
void gemcx_Config_fromFile(const char *filePath);

#endif // GEMCX_CONFIG_H

