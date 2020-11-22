#include "util/ex.h"

#include <string.h>

uint32_t
util_ex_rmchs(char *str, const uint32_t strSize, const char *rmChs,
		const bool cleanDoubleWS)
{
	const uint32_t rmChsSize = strlen(rmChs);
	uint32_t k = 0;
	char prevCh = ' ';
	for (uint32_t i = 0; i < strSize; ++i)
	{
		bool rmCh = false;

		if (cleanDoubleWS && (prevCh == ' ' && str[i] == ' '))
		{
			rmCh = true;
		}
		else
		{
			for (uint32_t j = 0; j < rmChsSize; ++j)
			{
				if (str[i] == rmChs[j])
				{
					rmCh = true;
					break;
				}
			}
		}

		if (!rmCh)
		{
			str[k++] = str[i];
		}

		prevCh = str[i];
	}
	str[k] = '\0';
	return k;
}

