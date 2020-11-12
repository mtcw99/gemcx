#ifndef PROTOCOL_GEMINI_HEADER_H
#define PROTOCOL_GEMINI_HEADER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

enum p_gemini_Header_StatusCode
{
	P_GEMINI_HEADER_STATUSCODE_INPUT = 1,
	P_GEMINI_HEADER_STATUSCODE_SUCCESS = 2,
	P_GEMINI_HEADER_STATUSCODE_REDIRECT = 3,
	P_GEMINI_HEADER_STATUSCODE_TEMPORARY_FAILURE = 4,
	P_GEMINI_HEADER_STATUSCODE_PERMANENT_FAILURE = 5,
	P_GEMINI_HEADER_STATUSCODE_CLIENT_CERTIFICATE_REQUIRED = 6,

	P_GEMINI_HEADER_STATUSCODE__TOTAL
};

struct p_gemini_Header
{
	char statusCode[3];
	char meta[1025];
};

void p_gemini_Header_get(struct p_gemini_Header *header,
		FILE *fp);
void p_gemini_Header_print(const struct p_gemini_Header *header);
bool p_gemini_Header_isGemini(const struct p_gemini_Header *header);
const char *p_gemini_Header_getStatusCodeStr(const struct p_gemini_Header *header);

#endif // PROTOCOL_GEMINI_HEADER_H

