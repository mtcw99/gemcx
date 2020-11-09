#ifndef GEMINI_HEADER_H
#define GEMINI_HEADER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

enum gemini_Header_StatusCode
{
	GEMINI_HEADER_STATUSCODE_INPUT = 1,
	GEMINI_HEADER_STATUSCODE_SUCCESS = 2,
	GEMINI_HEADER_STATUSCODE_REDIRECT = 3,
	GEMINI_HEADER_STATUSCODE_TEMPORARY_FAILURE = 4,
	GEMINI_HEADER_STATUSCODE_PERMANENT_FAILURE = 5,
	GEMINI_HEADER_STATUSCODE_CLIENT_CERTIFICATE_REQUIRED = 6,

	GEMINI_HEADER_STATUSCODE__TOTAL
};

struct gemini_Header
{
	char statusCode[3];
	char meta[1025];
};

void gemini_Header_get(struct gemini_Header *header,
		FILE *fp);
void gemini_Header_print(const struct gemini_Header *header);
bool gemini_Header_isGemini(const struct gemini_Header *header);
const char *gemini_Header_getStatusCodeStr(const struct gemini_Header *header);

#endif // GEMINI_HEADER_H

