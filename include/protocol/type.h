#ifndef PROTOCOL_TYPE_H
#define PROTOCOL_TYPE_H

#include <assert.h>

enum protocol_Type
{
	PROTOCOL_TYPE_GEMINI = 0,
	PROTOCOL_TYPE_GOPHER,

	PROTOCOL_TYPE__TOTAL
};

#define protocol_Type_assert(type) assert(type != PROTOCOL_TYPE__TOTAL)

#endif // PROTOCOL_TYPE_H

